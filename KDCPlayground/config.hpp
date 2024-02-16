#pragma once
struct _GlobalConfig {
	std::string server_address;
	std::string username;
} GlobalConfig;

constexpr const char* config_file_name = "multiplayer.json";
inline auto config_get_default() -> nlohmann::json {
	return R"({
		"username": "Unknown",
		"ip": "127.0.0.1"
	})"_json;
}

inline auto get_file_content(const std::string & file_name) -> std::string {
	std::ifstream ifs(file_name);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));

	return content;
}

inline auto file_exist(std::string file_name) {
	std::ifstream infile(file_name);
	return infile.good();
}

inline auto config_get() -> void {

	nlohmann::json json_config;
	if (file_exist(config_file_name)) {
		json_config = nlohmann::json::parse(get_file_content(config_file_name));
		printf("[*] Loading config\n");
	}
	else {
		json_config = config_get_default();
		std::ofstream o(config_file_name);
		o << std::setw(4) << json_config << std::endl;
                printf("[*] Config doesn't exist ! using default !\n");
	}
	
	GlobalConfig.server_address = json_config["ip"].get<std::string>();
	GlobalConfig.username = json_config["username"].get<std::string>();

        printf("KCD MP\n");
	printf("Server: %s\n", GlobalConfig.server_address.c_str());
	printf("Nickname: %s\n", GlobalConfig.username.c_str());
	printf("Build: %s %s\n", __DATE__, __TIME__);
}