#include <iostream>
#include <string>

#include "FileOperations/FileOperations.h"
#include "Encrypt/Encrypt.h"
#include "Decrypt/Decrypt.h"

#define LOG_STRING(x) printf("%s",x)
#define LOG_STRING_NL(x) printf("%s\n",x)

#define DECRYPT_FILE 1
#define ENCRYPT_FILE 2

void printHelp()
{
	LOG_STRING_NL("Usage: TestProgram.exe [Options] [FileName]");
	LOG_STRING_NL("--decrypt - For Decrypting Save Files");
	LOG_STRING_NL("--encrypt - For Re-Encrypting Save Files");
	LOG_STRING_NL("Or you can simply hover your files on to exe");
}

// Reference: https://stackoverflow.com/a/6417908
inline std::string remove_extension(const std::string& filename) {
	const size_t lastdot = filename.find_last_of('.');
	if (lastdot == std::string::npos) return filename;
	return filename.substr(0, lastdot);
}

void EncryptSH2Save(SaveFileBlocks& file_data, SaveFileThings& save_things)
{
	const auto encrypt = std::make_unique<Encrypt>(file_data, save_things);
	encrypt->run_process();
}

void DecryptSH2Save(SaveFileBlocks& file_data, SaveFileThings& saveThings)
{
	const auto decrypt = std::make_unique<Decrypt>(file_data,&saveThings);
	decrypt->run_process();
}

void StartSaveProcess(SaveFileBlocks &file_data, const char* file_name, const u8 type)
{
	SaveFileThings saveThings{};
	std::string name_buffer = file_name;
	std::string hash_name = file_name;
	
	switch (type)
	{
	case 1:
		DecryptSH2Save(file_data, saveThings);
		name_buffer = name_buffer + ".decrypted";
		if(!file_operations::write_file(file_data, name_buffer))
		{
			break;
		}
		hash_name = hash_name + ".hashkey";
		if(!file_operations::write_file(saveThings, hash_name))
		{
			break;
		}

		LOG_STRING_NL("Successfully Decrypted!");
		break;
	case 2:
		hash_name = remove_extension(hash_name);
		hash_name = hash_name + ".hashkey";
		if(!file_operations::read_file(saveThings, hash_name))
		{
			break;
		}

		EncryptSH2Save(file_data, saveThings);
		DecryptSH2Save(file_data, saveThings);
		EncryptSH2Save(file_data, saveThings);
		name_buffer = remove_extension(name_buffer);
		name_buffer = name_buffer + ".encrypted";
		if(file_operations::write_file(file_data, name_buffer))
		{
			break;
		}

		LOG_STRING_NL("Successfully Encrypted!");
		break; 
	default:
		LOG_STRING_NL("Wrong Choice!");
		break;
	}
}

int main(int argc, char* argv[])
{
	SaveFileBlocks file_data = {};

	if (argc == 3)
	{
		const std::string arg1(argv[1]);

		if (arg1 == "--decrypt" || arg1 == "-d")
		{
			if(!file_operations::read_file(file_data,argv[2]))
			{
				return 0;
			}
			StartSaveProcess(file_data, argv[2], DECRYPT_FILE);
		}
		if (arg1 == "--encrypt" || arg1 == "-e")
		{
			if (!file_operations::read_file(file_data, argv[2]))
			{
				return 0;
			}
			StartSaveProcess(file_data, argv[2], ENCRYPT_FILE);
		}
	}
	else if(argc == 2)
	{
		LOG_STRING_NL("What do you want to do with file you specified for?\n1-) For Decryption\n2-) For Encryption\n");
		LOG_STRING("Selection: ");
		int answer = 0;
		std::cin >> answer;

		if(answer == 1)
		{
			if (!file_operations::read_file(file_data, argv[1]))
			{
				return 0;
			}
			StartSaveProcess(file_data, argv[1], DECRYPT_FILE);
		}
		else if(answer == 2)
		{
			if(!file_operations::read_file(file_data, argv[1]))
			{
				return 0;
			}
			StartSaveProcess(file_data, argv[1], ENCRYPT_FILE);
		}
		else
		{
			printHelp();
		}
	}
	else
	{
		printHelp();
	}

	#if _WIN32
		system("PAUSE");
	#endif
;	return 0;
}