#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>

using namespace std;

// 哈夫曼树节点
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char character, int frequency) : ch(character), freq(frequency), left(nullptr), right(nullptr) {}
};

// 自定义比较器（小顶堆）
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

// 使用优先队列构建哈夫曼树
HuffmanNode* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;

    // 将频率表中的每个字符插入优先队列
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    // 构建哈夫曼树
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    return pq.top();
}

// 遍历哈夫曼树，生成字符编码表
void generateHuffmanCodes(HuffmanNode* root, string code, unordered_map<char, string>& huffmanCodes) {
    if (!root) return;

    // 如果是叶子节点
    if (!root->left && !root->right) {
        huffmanCodes[root->ch] = code;
    }

    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// 释放哈夫曼树
void freeHuffmanTree(HuffmanNode* root) {
    if (!root) return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    delete root;
}

// 将文本编码为二进制字符串
string encodeText(const string& text, const unordered_map<char, string>& huffmanCodes) {
    string encodedText;
    for (char ch : text) {
        encodedText += huffmanCodes.at(ch);
    }
    return encodedText;
}

// 将二进制字符串写入文件
void writeEncodedFile(const string& encodedText, const string& outputFile) {
    ofstream outFile(outputFile, ios::binary);
    bitset<8> bits;
    int bitCount = 0;

    for (char bit : encodedText) {
        bits[bitCount++] = (bit == '1');
        if (bitCount == 8) {
            outFile.put(static_cast<char>(bits.to_ulong()));
            bits.reset();
            bitCount = 0;
        }
    }

    // 写入剩余的位（如果有）
    if (bitCount > 0) {
        outFile.put(static_cast<char>(bits.to_ulong()));
    }

    outFile.close();
}

// 解码二进制文件为文本
string decodeText(const string& encodedText, HuffmanNode* root) {
    string decodedText;
    HuffmanNode* currentNode = root;

    for (char bit : encodedText) {
        currentNode = (bit == '0') ? currentNode->left : currentNode->right;

        // 如果到达叶子节点
        if (!currentNode->left && !currentNode->right) {
            decodedText += currentNode->ch;
            currentNode = root;
        }
    }

    return decodedText;
}

// 读取文件内容
string readFile(const string& fileName) {
    ifstream inFile(fileName);
    stringstream buffer;
    buffer << inFile.rdbuf();
    return buffer.str();
}

// 主函数
int main() {
    // 读取文本文件
    string inputFile = "war_and_peace.txt";
    string text = readFile(inputFile);

    // 统计字符频率
    unordered_map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }

    // 构建哈夫曼树
    HuffmanNode* root = buildHuffmanTree(freqMap);

    // 生成哈夫曼编码
    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // 输出字符和对应的编码
    cout << "Character Huffman Codes:\n";
    for (const auto& pair : huffmanCodes) {
        cout << pair.first << ": " << pair.second << "\n";
    }

    // 编码文本
    string encodedText = encodeText(text, huffmanCodes);

    // 写入编码文件
    string encodedFile = "encoded.bin";
    writeEncodedFile(encodedText, encodedFile);

    // 计算压缩比
    streampos originalSize = text.size() * 8; // 原始大小（字节数转为比特数）
    streampos compressedSize = encodedText.size(); // 压缩后大小（比特数）
    cout << "Original Size (bits): " << originalSize << "\n";
    cout << "Compressed Size (bits): " << compressedSize << "\n";
    cout << "Compression Ratio: " << (double)compressedSize / originalSize << "\n";

    // 解码文本
    string decodedText = decodeText(encodedText, root);

    // 输出第四章的内容
    size_t chapterPos = decodedText.find("CHAPTER IV");
    if (chapterPos != string::npos) {
        size_t chapterEnd = decodedText.find("CHAPTER V", chapterPos);
        string chapterContent = decodedText.substr(chapterPos, chapterEnd - chapterPos);
        cout << "Chapter IV Content:\n" << chapterContent << "\n";
    }
    else {
        cout << "Chapter IV not found.\n";
    }

    // 释放哈夫曼树
    freeHuffmanTree(root);

    return 0;
}
