#include "ADTAreaIDFix.h"

int main()
{
    string pathRoot = "root", pathOut = "out/";
    vector<string> fileList, fileOutList;
    uint32 fileCount = 0, i = 0, j = 0;

    for (const auto& entry : filesystem::directory_iterator(pathRoot))
    {
        fileList.emplace_back(entry.path().string());
        string fileName = pathOut + entry.path().stem().string() + ".adt";
        fileOutList.emplace_back(fileName);
        ++fileCount;
        cout << "=";
    }

    for (i = 0; i < fileCount; ++i)
    {
        FILE* rootADT = nullptr;
        FILE* outADT = nullptr;
        const char* rootName = fileList[i].c_str();
        const char* outName = fileOutList[i].c_str();
        vector<uint32> rootAreaID, outAreaID, outAreaIdOffs;
        uint32 rootMcnkOffs = 0, rootAreaIdOffs = 0;

        fopen_s(&rootADT, rootName, "rb");
        fopen_s(&outADT, outName, "r+b");

        fseek(outADT, 0x5C, SEEK_SET);              // first MCIN entry

        for (j = 0; j < 256; j++)
        {
            uint32 offset = ReadUInt32(outADT) + 60;
            outAreaIdOffs.emplace_back(offset);
            fseek(outADT, ftell(outADT) + 12, SEEK_SET);
        }

        cout << "=";
        rootMcnkOffs = FindFirstMCNKOffset(rootADT);

        for (j = 0; j < 256; ++j)
        {
            uint32 areaID = 0;
            rootAreaIdOffs = rootMcnkOffs + 60;
            fseek(rootADT, rootMcnkOffs + 4, SEEK_SET);
            rootMcnkOffs += ReadUInt32(rootADT) + 8;
            fseek(rootADT, rootAreaIdOffs, SEEK_SET);
            areaID = ReadUInt32(rootADT);
            rootAreaID.emplace_back(areaID);
        }

        cout << "=";

        for (j = 0; j < 256; j++)
        {
            fseek(outADT, outAreaIdOffs[j], SEEK_SET);
            uint32 aID = rootAreaID[j];
            fwrite(&aID, 4, 1, outADT);
        }

        cout << "=";
        _fcloseall();
    }

    return -1;
}

uint32 FindFirstMCNKOffset(FILE* filename)
{
    // I should probably avoid hardcoding that and instead read MVER and MHDR sizes
    // but from what I remember seeing in those block in files, they were always 0xC and 0x48
    uint32 offset = 0x54;
    uint32 tmp = 0;

    fseek(filename, offset, SEEK_SET);
    tmp = ReadUInt32(filename);

    while (tmp != 1296256587 && !feof(filename))    // tmp != MCNK
    {
        offset += ReadUInt32(filename) + 8;
        fseek(filename, offset, SEEK_SET);
        tmp = ReadUInt32(filename);
    }

    return offset;
}

uint32 ReadUInt32(FILE* filename)
{
    uint32 tmp = 0;
    fread(&tmp, 4, 1, filename);
    return tmp;
}
