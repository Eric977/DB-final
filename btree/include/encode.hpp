#include <vector>
#include <lz4.h>
#include <snappy.h>

enum CompressType {
    LZ4,
    SNAPPY
};

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

        std::cout << "Compressed size: " << compressedSize << "\n";
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

        std::cout << "Decompressed size: " << decompressedSize << "\n";
        return decompressed;
    }
    else {
        throw std::invalid_argument("lz4_decompress: unsupported type");
    }
}

template<typename T>
std::vector<char> snappy_compress(std::vector<T>& data) {
    if (std::is_same<T, std::pair<std::string, std::string>>::value) {
        // preprocess data
        std::vector<char> formatted_data = data_to_char_array(data);

        std::string compressed;
        snappy::Compress(formatted_data.data(), formatted_data.size(), &compressed);

        std::vector<char> compressedVec(compressed.begin(), compressed.end());
        std::cout << "Compressed size: " << compressedVec.size() << "\n";
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

        std::cout << "Decompressed size: " << decompressed.size() * sizeof(T) << "\n";
        return decompressed;
    }
    else {
        throw std::invalid_argument("snappy_decompress: unsupported type");
    }
}

template<typename T>
std::vector<char> compress(CompressType compressType, std::vector<T>& data, int &srcSize) {
    switch (compressType) {
        case LZ4:
            return lz4_compress<T>(data, srcSize);
        case SNAPPY:
            return snappy_compress<T>(data);
        default:
            return lz4_compress<T>(data, srcSize);
    }
}

template<typename T>
std::vector<T> decompress(CompressType compressType, std::vector<char>& compressed, int originalSize, int vectorSize) {
    switch (compressType) {
        case LZ4:
            return lz4_decompress<T>(compressed, originalSize, vectorSize);
        case SNAPPY:
            return snappy_decompress<T>(compressed, originalSize, vectorSize);
        default:
            return lz4_decompress<T>(compressed, originalSize, vectorSize);
    }
}