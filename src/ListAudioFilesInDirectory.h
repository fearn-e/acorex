#pragma once

#include "ofMain.h"

class ListAudioFilesInDirectory {

public:

	ListAudioFilesInDirectory();
	~ListAudioFilesInDirectory();

	void init();
	
	void beginListing();

	bool process();

	void checkOneFolder(const string& path, const vector<string>& extensions, vector<ofFile>& files);

	string setDirectory();

	vector<ofFile> getAudioFiles();
	int getFolderCount();
	int getRemainingFolderCount();
	bool getValidDirectorySelected();

private:

	ofFile selectedDirectory;
	ofDirectory dir;

	vector<string> allowedExtensions;

	vector<ofFile> audioFiles;
	vector<ofFile> folders;

	int searchedFolders;

	bool bInProgress;
};

