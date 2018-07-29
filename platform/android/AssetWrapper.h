#pragma once

#include <iostream>
#include <istream>
#include <sstream>
#include <iterator>

#include <streambuf>
#include <string>
#include <stdio.h>
#include <stdio.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>


class AssetWrapper
{
private:

	AAssetManager* mgr = NULL;
	std::vector<char> buffer;
	
public:
	AssetWrapper(AAssetManager* mgr)
	{
		SetManager(mgr);
	}

	~AssetWrapper()
	{
		buffer.clear();
	}

	void SetManager(AAssetManager* mgr)
	{
		this->mgr = mgr;
		ALOGV("Set asset manager");
	}

	void GetFileStream(std::string dir, std::string name, std::stringstream& stream)
	{
		buffer.clear();

		ALOGV("GetFileStream()");

		AAssetDir* assetDir = AAssetManager_openDir(mgr, dir.c_str());


		const char* filename;
		bool found = false;

		while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
		{
			ALOGV("dir found");

			//search for desired file
			if(!strcmp(filename, name.c_str()))
			{
				ALOGV("file found");
				found = true;
				AAsset *asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);

				//holds size of searched file
				off64_t length = AAsset_getLength64(asset);
				//keeps track of remaining bytes to read
				off64_t remaining = AAsset_getRemainingLength64(asset);
				size_t Mb = 1000 *1024; // 1Mb is maximum chunk size for compressed assets
				size_t currChunk;
				buffer.reserve(length);

				//while we have still some data to read
				while (remaining != 0) 
				{
					//set proper size for our next chunk
					if(remaining >= Mb)
					{
						currChunk = Mb;
					}
					else
					{
						currChunk = remaining;
					}
					char chunk[currChunk];

					//read data chunk
					if(AAsset_read(asset, chunk, currChunk) > 0) // returns less than 0 on error
					{
						//and append it to our vector
						buffer.insert(buffer.end(),chunk, chunk + currChunk);
						remaining = AAsset_getRemainingLength64(asset);
					}
				}
				AAsset_close(asset);
			}
		}

		if (!found)
		{
			ALOGE("GetFileStream() file not found");

		}
		else{
			ALOGE("GetFileStream() file found");
			//stream.rdbuf()->pubsetbuf(reinterpret_cast<char*>(&buffer[0]), buffer.size());
			std::copy(buffer.begin(), buffer.end(),std::ostream_iterator<char>(stream));
		}

		ALOGV("GetFileStream() returning");
	}
};
