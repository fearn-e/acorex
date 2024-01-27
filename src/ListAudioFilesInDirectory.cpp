#include "ListAudioFilesInDirectory.h"

ListAudioFilesInDirectory::ListAudioFilesInDirectory() {
	allowedExtensions = { "mp3", "ogg", "wav", "flac" };
	init();
}

ListAudioFilesInDirectory::~ListAudioFilesInDirectory() {}

void ListAudioFilesInDirectory::init() {
	dir.close();
	folders.clear();
	audioFiles.clear();
	searchedFolders = 0;
}

void ListAudioFilesInDirectory::beginListing() {
	init();
	folders.push_back(selectedDirectory);
}

bool ListAudioFilesInDirectory::process() {
	if (folders.size() > 0)
	{
		string path = folders.back().getAbsolutePath();
		folders.pop_back();

		checkOneFolder(path, { "" }, folders);
		checkOneFolder(path, allowedExtensions, audioFiles);

		searchedFolders++;
		
		inProgress = true;
	}
	else
	{
		inProgress = false;
	}

	return inProgress;
}

void ListAudioFilesInDirectory::checkOneFolder(const string& path, const vector<string>& extensions, vector<ofFile>& files) {
	dir.extensions.clear();
	dir.extensions = extensions;
	dir.listDir(path);
	if (dir.getFiles().size() > 0)
		files.insert(files.end(), dir.getFiles().begin(), dir.getFiles().end());
}

string ListAudioFilesInDirectory::setDirectory() {
	ofFileDialogResult result = ofSystemLoadDialog("Select samples folder", true, ofFilePath::getAbsolutePath("samples"));
	if (result.bSuccess) {
		selectedDirectory = ofFile(result.getPath());
		return result.getName();
	}
	else
	{
		return "";
	}
}

vector<ofFile> ListAudioFilesInDirectory::getAudioFiles() {
	return audioFiles;
}

int ListAudioFilesInDirectory::getFolderCount() {
	return searchedFolders;
}

int ListAudioFilesInDirectory::getRemainingFolderCount() {
	return folders.size();
}

bool ListAudioFilesInDirectory::getValidDirectorySelected() {
	return (selectedDirectory.exists() && selectedDirectory.getAbsolutePath() != "");
}