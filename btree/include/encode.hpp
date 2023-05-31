# include <vector>
# include <lz4.h>
# include <snappy.h>

enum CompressType {
    LZ4,
    SNAPPY
};

template<typename T>
std::vector<char> lz4_compress(std::vector<T>& data) {
    int srcSize = data.size() * sizeof(T);
    int maxCompressedSize = LZ4_compressBound(srcSize);
    std::vector<char> compressed(maxCompressedSize);

    int compressedSize = LZ4_compress_default(
        reinterpret_cast<const char*>(data.data()), 
        compressed.data(), 
        srcSize, 
        maxCompressedSize
    );

    compressed.resize(compressedSize);
    std::cout << "Compressed size: " << compressedSize << "\n";
    return compressed;
}

template<typename T>
std::vector<T> lz4_decompress(std::vector<char>& compressed, int originalSize) {
    std::vector<T> decompressed(originalSize);

    int decompressedSize = LZ4_decompress_safe(
        compressed.data(), 
        reinterpret_cast<char*>(decompressed.data()), 
        compressed.size(), 
        originalSize * sizeof(T)
    );
    std::cout << "Decompressed size: " << decompressedSize << "\n";

    return decompressed;
}

template<typename T>
std::vector<char> snappy_compress(std::vector<T>& data) {
    std::string input(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(T));
    std::string compressed;
    snappy::Compress(input.data(), input.size(), &compressed);
    // std::cout << input.size() << "\n";

    std::vector<char> compressedVec(compressed.begin(), compressed.end());
    std::cout << "Compressed size: " << compressedVec.size() << "\n";
    return compressedVec;
}

template<typename T>
std::vector<T> snappy_decompress(std::vector<char>& compressed, int originalSize) {
    std::string compressedStr(compressed.begin(), compressed.end());
    std::string decompressed;
    snappy::Uncompress(compressedStr.data(), compressedStr.size(), &decompressed);
    // std::cout << compressedStr.size() << "\n";

    std::vector<T> decompressedVec(originalSize);
    memcpy(decompressedVec.data(), decompressed.data(), originalSize * sizeof(T));
    std::cout << "Decompressed size: " << decompressedVec.size() * sizeof(T) << "\n";

    return decompressedVec;
}

template<typename T>
std::vector<char> compress(CompressType compressType, std::vector<T>& data) {
    switch (compressType) {
        case LZ4:
            return lz4_compress<T>(data);
        case SNAPPY:
            return snappy_compress<T>(data);
        default:
            return lz4_compress<T>(data);
    }
}

template<typename T>
std::vector<T> decompress(CompressType compressType, std::vector<char>& compressed, int originalSize) {
    switch (compressType) {
        case LZ4:
            return lz4_decompress<T>(compressed, originalSize);
        case SNAPPY:
            return snappy_decompress<T>(compressed, originalSize);
        default:
            return lz4_decompress<T>(compressed, originalSize);
    }
}