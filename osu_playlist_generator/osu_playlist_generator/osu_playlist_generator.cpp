#include <boost/filesystem.hpp>
#include <iostream>
std::string get_audio_name(boost::filesystem::path path, bool& audio_found) {
	boost::filesystem::ifstream osu_file;
	std::string audio_filename;
	osu_file.open(path);
	for (std::string line; std::getline(osu_file, line); ) {
		if (line.find("AudioFilename:") != std::string::npos) {
			audio_filename = line;
			audio_filename.erase(begin(audio_filename), (begin(audio_filename) + 15));
			audio_found = true;
		}
	}
	osu_file.close();
	return audio_filename;
}
int main() {
	std::cout << "Osu Playlist Generator\n";
	std::cout << "Checking Songs directory...";
	boost::filesystem::path working_directory = boost::filesystem::system_complete("Songs");
	if (!exists(working_directory)) {
		std::cout << " failed.\nRun program from within osu! folder.\nExiting.\n";
		return -1;
	}
	std::vector<std::string> songs_dirs;
	for (auto& it : boost::filesystem::directory_iterator(working_directory)) {
		if (boost::filesystem::is_directory(it)) {
			boost::filesystem::path relative_path = relative(it.path(), working_directory);
			songs_dirs.push_back(relative_path.string());
		}
	}
	if (!songs_dirs.size()) {
		std::cout << "Songs directory is empty.\n Exiting...\n";
		return -2;
	}
	std::sort(begin(songs_dirs), end(songs_dirs));
	songs_dirs.erase(remove_if(begin(songs_dirs), end(songs_dirs), [](std::string& elem) {return !(elem.at(0) >= '0' && elem.at(0) <= '9'); }), end(songs_dirs));
	if (!songs_dirs.size()) {
		std::cout << "Didn't find any songs.\n Exiting...\n";
		return -3;
	}
	std::cout << " succeded.\n";
	std::cout << songs_dirs.size() << " entries found.\n";
	std::cout << "Generating playlist...\n";
	std::ofstream playlist_file;
	playlist_file.open("playlist.m3u");
	for (auto& it : songs_dirs) {
		bool audio_found = false;
		boost::filesystem::path song_dir = working_directory.string() + "\\" + it.c_str();
		for (auto& p : boost::filesystem::directory_iterator(song_dir)) {
			if (p.path().extension() == ".osu" && !audio_found) {
				std::string audio_name = get_audio_name(p.path(), audio_found);
				playlist_file << song_dir.string() << "\\" << audio_name << "\n";
			}
		}
	}
	std::cout << "Playlist sucessfully generated.\n";
	std::cout << "\n";
	playlist_file.close();
	return 0;
}