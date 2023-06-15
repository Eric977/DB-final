#include <vector>
#include <lz4.h>
#include <snappy.h>
#include <codecfactory.h>
#include <deltautil.h>

// data to char array
std::vector<char> data_to_char_array(std::vector<std::pair<std::string, std::string>>& data) {
    std::vector<char> formatted_data;
    
    for (const auto& val : data) {
        formatted_data.insert(
            formatted_data.end(),
            val.first.begin(),
            val.first.end()
        );
        formatted_data.push_back('\0');
        formatted_data.insert(
            formatted_data.end(),
            val.second.begin(),
            val.second.end()
        );
        formatted_data.push_back('\0');
    }
    
    return formatted_data;
}

// char array to data
std::vector<std::pair<std::string, std::string>> char_array_to_data(std::vector<char>& formatted_decompressed, int vectorSize) {
    std::vector<std::pair<std::string, std::string>> decompressed;
    int index = 0;
    
    for (int i = 0; i < vectorSize; ++i) {
        std::string first(formatted_decompressed.data() + index);
        index += first.size() + 1;
        std::string second(formatted_decompressed.data() + index);
        index += second.size() + 1;
        decompressed.emplace_back(std::move(first), std::move(second));
    }

    return decompressed;
}

// convert vector string to uint32_t
void strToUint32(std::vector<uint32_t> &vstr, std::vector<std::string> strs, int strSize){
    for (std::string str : strs) {       
        for (int i = 0; i < strSize; i += 4){
            uint32_t tmp = 0;
            for (int j = 0; j < 4; j++){
                tmp = tmp << 8;
                if (i + j < str.size())
                    tmp += str[i + j];
                else
                    tmp += 0;
            }
            vstr.push_back(tmp);
        }
    }
}

// convert vector uint32_t to string
void uint32ToStr(std::vector<uint32_t> vstr, std::vector<std::string> &str, int strSize){
    int strLen = strSize/4;
    
    for (int i = 0; i < vstr.size(); i += strLen) {
        std::vector<uint32_t> tmpVec(vstr.begin() + i, vstr.begin() + i + strLen);
        std::string tmpStr;

        for (int i = 0; i < tmpVec.size(); i++){
            for (int j = 0; j < 4; j++){
                tmpStr += (char)(tmpVec[i] >> (24 - j * 8));
            }
        }

        str.push_back(tmpStr);
    }
}

// convert vector<pair<T, T>> to two vector
template<typename T>
void pairToTwoVec(std::vector<std::pair<T, T>> &vstr, std::vector<T> &v1, std::vector<T> &v2){
    for (int i = 0; i < vstr.size(); i ++){
        v1.push_back(vstr[i].first);
        v2.push_back(vstr[i].second);
    }
}

// convert two vector to vector<pair<T, T>>
template<typename T>
void twoVecToPair(std::vector<std::pair<T, T>> &vstr, std::vector<T> v1, std::vector<T> v2){
    for (int i = 0; i < v1.size(); i ++){
        vstr.push_back(std::make_pair(v1[i], v2[i]));
    }
}

template<typename T>
std::vector<char> lz4_compress(std::vector<T>& data, int& srcSize) {
    if (std::is_same<T, std::pair<std::string, std::string>>::value) {
        // preprocess data
        std::vector<char> formatted_data = data_to_char_array(data);
        
        srcSize = formatted_data.size();
        int maxCompressedSize = LZ4_compressBound(srcSize);
        std::vector<char> compressed(maxCompressedSize);
        
        // compress formatted_data
        int compressedSize = LZ4_compress_default(
            formatted_data.data(),
            compressed.data(),
            srcSize,
            maxCompressedSize
        );

        compressed.resize(compressedSize);

        std::cout << "Compressed size: " << compressedSize << " bytes" << "\n";
        return compressed;
    }
    else {
        throw std::invalid_argument("lz4_compress: unsupported type");
    }
}

template<typename T>
std::vector<T> lz4_decompress(std::vector<char>& compressed, int originalSize, int vectorSize) {
    if (std::is_same<T, std::pair<std::string, std::string>>::value) {
        // decompress data
        std::vector<char> formatted_decompressed(originalSize);
        int decompressedSize = LZ4_decompress_safe(
            compressed.data(),
            formatted_decompressed.data(),
            compressed.size(),
            originalSize
        );

        // postprocess data
        std::vector<T> decompressed = char_array_to_data(formatted_decompressed, vectorSize);

        std::cout << "Decompressed size: " << decompressedSize << " bytes" << "\n";
        return decompressed;
    }
    else {
        throw std::invalid_argument("lz4_decompress: unsupported type");
    }
}

template<typename T>
std::vector<char> snappy_compress(std::vector<T>& data, int& srcSize) {
    if (std::is_same<T, std::pair<std::string, std::string>>::value) {
        // preprocess data
        std::vector<char> formatted_data = data_to_char_array(data);
        srcSize = formatted_data.size();

        std::string compressed;
        snappy::Compress(formatted_data.data(), formatted_data.size(), &compressed);

        std::vector<char> compressedVec(compressed.begin(), compressed.end());
        std::cout << "Compressed size: " << compressedVec.size() << " bytes" << "\n";
        return compressedVec;
    }
    else {
        throw std::invalid_argument("snappy_compress: unsupported type");
    }
}

template<typename T>
std::vector<T> snappy_decompress(std::vector<char>& compressed, int originalSize, int vectorSize) {
    if (std::is_same<T, std::pair<std::string, std::string>>::value) {
        // decompress data
        std::string compressedStr(compressed.begin(), compressed.end());
        std::string decompressed_string;
        snappy::Uncompress(compressedStr.data(), compressedStr.size(), &decompressed_string);
        std::vector<char> formatted_decompressed(decompressed_string.begin(), decompressed_string.end());

        // postprocess data
        std::vector<T> decompressed = char_array_to_data(formatted_decompressed, vectorSize);

        std::cout << "Decompressed size: " << originalSize << " bytes" << "\n";
        return decompressed;
    }
    else {
        throw std::invalid_argument("snappy_decompress: unsupported type");
    }
}

template<typename T>
void fastpfor_compress(std::vector<uint32_t>& k, std::vector<uint32_t>& v, 
            std::vector<T>& data, int& srcSize){
    if (std::is_same<T, std::pair<std::string, std::string>>::value) {
        std::vector<std::pair<uint32_t, uint32_t>> uint_data;
        std::vector<uint32_t> uint_keys, uint_values;
        std::vector<std::string> keys, values;

        pairToTwoVec<std::string>(data, keys, values);

        strToUint32(uint_keys, keys, srcSize);
        strToUint32(uint_values, values, srcSize);

        // compress data
        using namespace FastPForLib;
        CODECFactory factory;
        IntegerCODEC &codec = *factory.getFromName("simdfastpfor256");

        std::vector<uint32_t> compressedKeys(uint_keys.size() + 1024);
        std::vector<uint32_t> compressedValues(uint_values.size() + 1024);
        size_t keysSize = compressedKeys.size();
        size_t valuesSize = compressedValues.size();

        codec.encodeArray(uint_keys.data(), uint_keys.size(), compressedKeys.data(),
                            keysSize);
        codec.encodeArray(uint_values.data(), uint_values.size(), compressedValues.data(),
                            valuesSize);
        
        compressedKeys.resize(keysSize);
        compressedValues.resize(valuesSize);
        compressedKeys.shrink_to_fit();
        compressedValues.shrink_to_fit();
        k = compressedKeys;
        v = compressedValues;

        // std::cout << compressedKeys.size() << " " << compressedValues.size() << std::endl;

        // display compression rate:
        std::cout << std::setprecision(3);
        std::cout << "You are using "
                    << 32.0 * static_cast<double>(compressedKeys.size() + compressedValues.size()) /
                        static_cast<double>(uint_keys.size() + uint_values.size())
                    << " bits per integer. " << std::endl;
    }
    else {
        throw std::invalid_argument("fastpfor_compress: unsupported type");
    }
}

template<typename T>
std::vector<T> fastpfor_decompress(std::vector<uint32_t> uint_keys, std::vector<uint32_t> uint_values, 
                                    int srcSize, int vectorSize){
    std::vector<std::string> keys, values;
    std::vector<T> data;

    // std::cout << uint_keys.size() << " " << uint_values.size() << "\n";

    // decompress data
    using namespace FastPForLib;
    CODECFactory factory;
    IntegerCODEC &codec = *factory.getFromName("simdfastpfor256");

    std::vector<uint32_t> originalKeys(vectorSize * srcSize / 4);
    std::vector<uint32_t> originalValues(vectorSize * srcSize / 4);
    size_t keysSize = originalKeys.size();
    size_t valuesSize = originalValues.size();

    codec.decodeArray(uint_keys.data(), uint_keys.size(), originalKeys.data(), keysSize);
    codec.decodeArray(uint_values.data(), uint_values.size(), originalValues.data(), valuesSize);
    
    originalKeys.resize(keysSize);
    originalValues.resize(valuesSize);

    uint32ToStr(originalKeys, keys, srcSize);
    uint32ToStr(originalValues, values, srcSize);

    twoVecToPair<std::string>(data, keys, values);
    
    return data;
}