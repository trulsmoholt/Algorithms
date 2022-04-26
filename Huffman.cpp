#include <iostream>
#include <functional>
#include <queue>
#include <vector>
#include <map>
class Node {
public:
	const Node* _left;
	const Node* _right;
	float _probability;
	char _symbol;

	Node(const Node* left, const Node* right) : _left(left), _right(right), _probability(left->_probability+right->_probability), _symbol(NULL) {};
	Node(const char symbol, float probability) : _symbol(symbol), _probability(probability),_left(nullptr),_right(nullptr) {};
};

class Compare {
public:
	bool operator()(Node* left, Node* right) { return left->_probability > right->_probability; };
};

std::map<char, int> freqTable(const std::string &data) {
	std::map<char, int> m;

	for (int i = 0; i < data.length();i++) {
		char c = data.at(i);
		if (m.find(c) == m.end()) {
			for (int j = i; j < data.length();j++) {
				if (data.at(j) == c) {
					m[c]++;
				}
			}
		}
	}
	return m;
}
std::map<char, float> probabilityTable(const std::map<char,int> &freq) {
	std::map<char, float> m;
	int data_length = 0;
	for (auto& entry : freq) {
		data_length += entry.second;
	}

	for (auto &entry : freq) {
		char symbol = entry.first;
		float frequency = (float)freq.at(symbol);
		m[symbol] = frequency / data_length;
	}
	return m;
}
void print_queue(std::priority_queue< Node*, std::vector<Node*>, Compare > q) { // NB: pass by value so the print uses a copy
	while (!q.empty()) {
		std::cout << q.top()->_probability << ' ';
		q.pop();
	}
	std::cout << '\n';
}
void renderTable(std::map<char, std::vector<bool>> &table, const Node *node, std::vector<bool> code) {
	if (node->_left != nullptr) {
		std::vector<bool> leftcode(code);
		leftcode.push_back(false);
		renderTable(table, node->_left, leftcode);
	}
	else {
		table[node->_symbol] = code;
	}
	if (node->_right != nullptr) {
		std::vector<bool> rightcode(code);
		rightcode.push_back(true);
		renderTable(table, node->_right, rightcode);
	}
	else {
		table[node->_symbol] = code;
	}
}
std::map<char, std::vector<bool>> huffmanEncode(const std::string& data) {
	auto frequencies = freqTable(data);
	auto probabilities = probabilityTable(frequencies);

	void* raw_memory = operator new[](sizeof(Node)*probabilities.size()* probabilities.size());
	Node* nodes = static_cast<Node*>(raw_memory);
	int count = 0;

	std::priority_queue < Node*, std::vector<Node*>, Compare > q;
	for (auto entry : probabilities) {
		Node* node = new Node(entry.first, entry.second);
		nodes[count++] = *node;
		q.push(node);
	}
	while (q.size() > 1) {
		Node* left = q.top();
		q.pop();
		Node* right = q.top();
		q.pop();
		Node* new_internal_node = new Node(left, right);
		nodes[count++] = *new_internal_node;
		q.push(new_internal_node);
		print_queue(q);
	}
	std::map<char, std::vector<bool>> m;
	renderTable(m, q.top(), std::vector<bool> {});
	delete[] nodes;
	return m;
}
std::vector<bool> encoder(std::string input, std::map<char, std::vector<bool>> huffman_codes) {
	std::vector<bool> output;
	for (auto& symbol : input) {
		output.insert(output.end(), huffman_codes[symbol].begin(), huffman_codes[symbol].end());
	}
	return output;
}
int main() {
	std::string sample_data = "Hello world";
	std::map<char, std::vector<bool>> codes = huffmanEncode(sample_data);
	for (auto& entry : codes) {
		std::cout << entry.first<<" : ";
		for (bool bit : entry.second) {
			std::cout << bit ? 1 : 0;
		}
		std::cout << std::endl;
	}
	auto encoding = encoder(sample_data, codes);
	std::cout << sample_data << std::endl << "===" << std::endl;
	for (bool bit : encoding) {
		std::cout << bit ? 1 : 0;
	}
}
