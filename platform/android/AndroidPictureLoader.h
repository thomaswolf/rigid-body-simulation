#pragma once

// based on: http://cs.dvc.edu/HowTo_Cparse.html

#include <iostream>
#include <fstream>
#include <string>
#include <iostream>


#include "AssetWrapper.h"

#define MAX_CHARS_PER_LINE_IMG 50
#define MAX_TOKENS_PER_LINE_IMG 10
#define DELIMITER_IMG ","

class AndroidPictureLoader
{

private:
	const char* line[MAX_CHARS_PER_LINE_IMG] = {};
	char buf[MAX_CHARS_PER_LINE_IMG];
	AssetWrapper* assets;

public:
	std::vector<std::vector<vec3> > rgbValues;
	int xRes;
	int yRes;
	int rgbSize = 255;

	bool LoadPicture(AssetWrapper* assets, std::string name)
	{
		this->assets = assets;
		loadFile(name  + ".txt");
	}

private:
	std::vector<std::vector<vec3> > loadFile(std::string name)
	{
		
		// create a file-reading object
		//std::ifstream fin;
		//fin.open(name); // open a file
		std::stringstream fin;
		assets->GetFileStream("", name, fin);
		std::cout << "open image/txt file: " << name << std::endl;
		ALOGV("open picture");
		if (!fin.good()){
			ALOGE("picture not found");
			assert(false && "file not found");
		}

		int lineCount = 0;
		// read each line of the file
		while (!fin.eof())
		{
			// read an entire line into memory
			clearBuf();
			fin.getline(buf, MAX_CHARS_PER_LINE_IMG);

			// parse the line into blank-delimited tokens
			int n = 0; // a for-loop index
			
			// parse the line
			line[0] = strtok(buf, DELIMITER_IMG); // first line
			if (line[0]) // zero if line is blank
			{
				for (n = 1; n < MAX_TOKENS_PER_LINE_IMG; n++)
			  	{
					line[n] = strtok(0, DELIMITER_IMG); // subsequent tokens

					if (!line[n]) break; // no more tokens
			  	}
			  	if(lineCount == 0)	// header cae
			  	{
					xRes = std::atoi(line[0]);
					yRes = std::atoi(line[1]);
					rgbSize = std::atoi(line[2]);
					// resize Vector
					rgbValues.resize(xRes);
					for(int i = 0; i < xRes; ++i)
					{
						rgbValues[i].resize(yRes);
					}
				}
				else
				{
					int x = std::atoi(line[0]);
					int y = std::atoi(line[1]);
					float R = std::atoi(line[2])/float(rgbSize);
					float G = std::atoi(line[3])/float(rgbSize);
					float B = std::atoi(line[4])/float(rgbSize);
					rgbValues[x][-y+(yRes-1)] = vec3(R,G,B);
				}

			}
			++lineCount;
		}


		return rgbValues;
	}
	
	void clearBuf()
	{
		memset(buf, 0, sizeof(buf));
	}
};
