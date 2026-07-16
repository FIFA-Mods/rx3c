#include "Rx3Names.h"
#include "errormsg.h"

vector<pair<unsigned int, string>> ExtractNamesFromChunk(Rx3Chunk const *namesChunk) {
    vector<pair<unsigned int, string>> result;
    Rx3Reader reader(namesChunk);
    reader.Skip(4);
    unsigned int numNames = reader.Read<unsigned int>();
    reader.Skip(8);
    for (unsigned int i = 0; i < numNames; i++) {
        unsigned int id = reader.Read<unsigned int>();
        unsigned int nameLength = reader.Read<unsigned int>();
        result.emplace_back(id, reader.GetString());
        reader.Skip(nameLength);
    }
    return result;
}
