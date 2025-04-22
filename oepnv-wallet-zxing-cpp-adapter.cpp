#include "base64.h"

#include "ReadBarcode.h"
#include "BitMatrix.h"
#include "MultiFormatWriter.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace ZXing;


/// Dieser Adapter ermöglicht der Open-Source-Plattform "ÖPNV-Wallet" den Zugriff auf ZXing-CPP-Funktionalitäten via CLI.
/// Es gibt zur Zeit leider keine native Swift-Library, die Aztec-Codes serverseitig en- und dekodieren kann.
/// Dieser Workaround stellt somit eine Übergangslösung dar, bis eine entsprechende Swift-Library veröffentlicht wird.


/// @brief Entfernt ungültige Zeichen aus einer Base64-Zeichenkette.
/// @param input Die Base64-Zeichenkette, die bereinigt werden soll.
/// @return Die bereinigte Base64-Zeichenkette.
std::string clean_base_64(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    std::copy_if(input.begin(), input.end(), std::back_inserter(result), [](char c) {
        return std::isalnum(c) || c == '+' || c == '/' || c == '=';
    });
    return result;
}


/// @brief Wrapper für die Base64-Encoding-Funktion von base64.cpp.
/// @param data Die Daten, die Base64-enkodiert werden sollen.
/// @return Eine Base64-Zeichenkette mit den enkodierten Daten.
std::string base_64_encode(const std::vector<uint8_t>& data) {
    return base64_encode(data.data(), data.size());
}


/// @brief Wrapper für die Base64-Decoding-Funktion von base64.cpp.
/// @param input Eine Base64-Zeichenkette mit den enkodierten Daten.
/// @return Die Daten, die zuvor Base64-enkodiert waren.
std::vector<uint8_t> base_64_decode(const std::string& input) {
    std::string decoded = base64_decode(input);
    return std::vector<uint8_t>(decoded.begin(), decoded.end());
}


/// @brief Dekodiert einen Aztec-Code und gibt dessen Inhalt zurück.
/// @param base64 Ein Base64-enkodiertes Bild eines Aztec-Codes.
/// @return Die Base64-enkodierten Daten des Inhalts des Aztec-Codes.
std::string decode_aztec_code_base_64(const std::string& base64) {
    std::vector<uint8_t> payload = base_64_decode(base64);
    
    int w, h, channels;
    uint8_t* imgData = stbi_load_from_memory(payload.data(), payload.size(), &w, &h, &channels, 1);
    if(!imgData) {
        std::cerr << "Fehler beim Laden des Bildes." << std::endl;
        exit(1);
    }

    ImageView image(imgData, w, h, ImageFormat::Lum);
    ZXing::ReaderOptions options;
    options.setFormats(ZXing::BarcodeFormat::Aztec);
    auto result = ReadBarcode(image, options);
    stbi_image_free(imgData);

    if(!result.isValid()) {
        std::cerr << "Kein Aztec-Code erkannt." << std::endl;
        exit(1);
    }

    return base_64_encode(result.bytes());
}


/// @brief Erzeugt einen Aztec-Code mit den übergebenen Daten als Inhalt.
/// @param base64 Die Base64-enkodierten Daten für den Inhalt des Aztec-Codes.
/// @return Das Base64-enkodierte Bild des Aztec-Codes.
std::string encode_aztec_code_base_64(const std::string& base64) {
    std::vector<uint8_t> payload = base_64_decode(base64);

    std::wstring binary;
    binary.reserve(payload.size());
    for(uint8_t b : payload)
        binary.push_back(static_cast<wchar_t>(b));

    auto matrix = MultiFormatWriter(BarcodeFormat::Aztec).setEncoding(CharacterSet::BINARY).encode(binary, 500, 500);

    int width = matrix.width();
    int height = matrix.height();
    std::vector<uint8_t> pixels(width * height);
    for(int y = 0; y < height; ++y)
        for(int x = 0; x < width; ++x)
            pixels[y * width + x] = matrix.get(x, y) ? 0 : 255;

    std::vector<uint8_t> png;
    stbi_write_png_to_func(
        [](void* context, void* data, int size) {
            auto* out = static_cast<std::vector<uint8_t>*>(context);
            out->insert(out->end(), (uint8_t*)data, (uint8_t*)data + size);
        },
        &png, width, height, 1, pixels.data(), width
    );

    return base_64_encode(png);
}


/// Beispielnutzung:
/// echo "<base64-data>" | ./oepnv-wallet-zxing-cpp-adapter aztec-encode => <base64-image>
/// echo "<base64-image>" | ./oepnv-wallet-zxing-cpp-adapter aztec-decode => <base64-data>
/// Die Nutzdatenübertragung läuft via STDIN + STDOUT und muss immer Base64-enkodiert sein!
int main(int argc, char* argv[]) {
    if (argc == 2) {
        std::string mode = argv[1];
        std::string base64_input((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
        std::string base64_input_cleaned = clean_base_64(base64_input);
        if (mode == "aztec-decode") {
            std::string result = decode_aztec_code_base_64(base64_input_cleaned);
            std::cout << result << std::endl;
            return 0;
        } else if (mode == "aztec-encode") {
            std::string result = encode_aztec_code_base_64(base64_input_cleaned);
            std::cout << result << std::endl;
            return 0;
        } else {
            std::cerr << "API falsch verwendet." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "API falsch verwendet." << std::endl;
        return 1;
    }
}