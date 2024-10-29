#include <iostream>

#include "EasyIPC/Client.h"
#include "EasyIPC/Encryption/AesEaxEncryptionStrategy.h"

#include <Windows.h>

#define KEY_G 0x47
#define KEY_H 0x48

bool isKeyPressed(int key)
{
	return GetAsyncKeyState(key) & 0x1;
}

int main()
{
	try
	{
		EasyIPC::Client client{};

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
		client.setEncryptionStrategy(std::make_shared<EasyIPC::AesEaxEncryptionStrategy>(aesKeyHex));

		// This feature has to be supported by the encryption strategy (AesEaxEncryptionStrategy does support it)
		// The overall encryption provides confidentiality but the EAX mode in the AES will
		// also provide authentication: If the message has even just one bit modified during traffic
		// then this callback will fire as the message authentication code (MAC) does not match anymore.
		client.setOnCompromisedCallback([]()
		{
			std::cerr << "Communication compromised!\n";
		});

		// if server emits "update-value" then call this callback
		client.on("update-value", [](const nlohmann::json& data)
		{
			// Easily access properties thanks to it being JSON
			int importantValue = data["someProperty"];

			// Do something with data...
			std::cout << "New important value from server: " << importantValue << "\n";
		});

		// You have to manually call this to actually connect,
		// For local communication between processes use tcp://localhost as url and any free port
		client.connect("tcp://localhost", 57239);

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			// emit something to server when you press G
			if (isKeyPressed(KEY_G))
			{
				// This call is synchronous and will return the response of the server
				nlohmann::json response = client.emit("greet", { {"someProp", true} });
				std::cout << "Server responded with: " << response.dump() << "\n";
			}

			// stop it all when pressing H
			if (isKeyPressed(KEY_H))
			{
				break;
			}

		}

	}
	catch (std::exception& exception)
	{
		std::cerr << "Client error: " << exception.what() << "\n";
	}

	return 0;
}