#include <string>
#include <cassert>

using namespace std;

int main(){
	
	string S = string("Testing");
	string T = string("Testing", 1);
	assert(T == S);
}
