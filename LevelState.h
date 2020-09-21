//Save, load, and hold the current level index and side
#include <iostream>
#include <fstream>
#include <vector>

#ifndef LEVELSTATE_H
#define LEVELSTATE_H

class LevelState {
	private:
	int index;
	int side;
	bool fileExists;
	std::string filename;
	
	void saveFile() {
		FILE *fp = fopen(filename.c_str(), "w");
		//nice and simple file structure
		putw(index,fp);
		putw(side,fp);
		fclose(fp);
		fileExists = true;
	}
	
	bool loadFile() {
		FILE *fp = fopen(filename.c_str(), "r");
		if(!fp) {
			return false;
		}
		else {
			index = getw(fp);
			side = getw(fp);
			fclose(fp);
			return true;
		}
	}
	
	public:
	LevelState(std::string filename) {
		this->filename = filename;
		//try to load the file from the given filename
		FILE *fp = fopen(filename.c_str(), "r");
		if(!fp) {
			//on failure, default to 0,0
			index = 0;
			side = 0;
			fileExists = false;
		}
		else {
			fclose(fp);
			loadFile();
			fileExists = true;
		}
	}
	~LevelState() {
		save();
	}
	
	int getIndex() {
		return index;
	}
	int getSide() {
		return side;
	}
	void setIndex(int newIndex) {
		index = newIndex;
	}
	bool doesFileExist() {
		return fileExists;
	}
	void setSide(int newSide) {
		side = newSide;
	}
	void save() {
		saveFile();
	}
	void deleteSave() {
		index = 0;
		side = 0;
		//remove file if it exists
		remove(filename.c_str());
		fileExists = false;
	}
};

#endif
