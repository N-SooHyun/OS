#pragma once

class FileFunc{
public:
	static FILE* RdFile(const char* rPath){
		FILE* rf;
		fopen_s(&rf, rPath, "r");
		if (rf == nullptr) std::cout << "FILE READ FAIL" << std::endl;
		return rf;
	}

	static FILE* WtFile(const char* wPath){
		FILE* wf;
		fopen_s(&wf, wPath, "w");
		if (wf == nullptr) std::cout << "FILE WRITE FAIL" << std::endl;
		return wf;
	}

	static FILE* aWtFile(const char* awPath){
		FILE* wf;
		fopen_s(&wf, awPath, "a");
		if (wf == nullptr) std::cout << "FILE WRITE FAIL" << std::endl;
		return wf;
	}

	static void FileToStr(FILE* rF, DynamicStr& Str){
		if (rF == nullptr){
			return;
		}
		char c = ' ';
		while (1){
			c = fgetc(rF);
			if (c == EOF) break;
			Str.Append_Char(&c);
		}
	}	
};