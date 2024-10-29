#include <iostream>

#include "EasyIPC/Server.h"
#include "EasyIPC/Encryption/AesEaxEncryptionStrategy.h"

#include <Windows.h>

bool isKeyPressed(int key)
{
	return GetAsyncKeyState(key) & 0x1;
}

int main()
{
	try
	{
		EasyIPC::Server server;

		// The provided encryption is symmetric, so you should make sure you have some
		// means to avoid having the shared key in plain text in the code that can be easily found by looking
		// at your programs strings.
		// Suggestion: Encrypt the strings somehow (either xor or more sophisticated approaches)
		// https://github.com/JustasMasiulis/xorstr
		// Example usage: std::string key = xorstr_("yourkey");
		std::string aesKeyHex = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4";

		// This is optional but if you dont set an encryption strategy then
		// the data will be sent in plain text and is easily readable in wireshark
		// If you dont care about that, prefer not setting one as encryption has a performance penalty.
		server.setEncryptionStrategy(std::make_shared<EasyIPC::AesEaxEncryptionStrategy>(aesKeyHex));

		// This feature has to be supported by the encryption strategy (AesEaxEncryptionStrategy does support it)
		// The overall encryption provides confidentiality but the EAX mode in the AES will
		// also provide authentication: If the message has even just one bit modified during traffic
		// then this callback will fire as the message authentication code (MAC) does not match anymore.
		server.setOnCompromisedCallback([]()
		{
			std::cerr << "Communication compromised!\n";
			// Do something about it... e.g. log to backend or shutdown immediately 
		});

		// if any clients sends greet event, this callback will be called
		server.on("greet", [&server](const nlohmann::json& data)
		{
			std::cout << "Got greet event with data: " << data.dump(4) << "\n";

			// For convenience: Returning from this handler callback will send this json back to the
			// original client who send the greet event
			return nlohmann::json{ {"someData", 10} };
		});

		// You need to call this to actually serve to clients
		// For local communication between processes use tcp://localhost as url and any free port
		server.serve("tcp://localhost", 57239);


		// Some random example value that we want to communicate to the clients
		int importantValue{ 1 };

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			// send the latest value to all connected clients when you press space
			if (isKeyPressed(VK_SPACE))
			{
				server.emit("update-value", {
					{"someProperty", importantValue}
				});
			}

			// we update the value constantly
			++importantValue;

			if (isKeyPressed(VK_ESCAPE))
				break;
		}

		// Technically you do not need to call this manually as its automatically
		// called for you in the destructor of EasyIPC::Server when the instance goes out of scope.
		// But if you want to for whatever reason shutdown the server earlier, this is how.
		server.shutdown();
	}
	catch (std::exception& exception)
	{
		std::cerr << "Server error: " << exception.what() << "\n";
	}

	return 0;
}
