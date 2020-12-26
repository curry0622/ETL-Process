#include<bits/stdc++.h>

using namespace std;

vector<string> inputVec;

void convertStr(string& inputStr, bool last = false) {
    // convert input string into JSON format
    string resultStr = "\t{\n\t\t\"col_1\":";
    int cnt = 1;
    int findSplit = inputStr.find("|");
    while(findSplit != string::npos) {
        cnt++;
        resultStr += (inputStr.substr(0, findSplit) + ",\n\t\t\"col_" + to_string(cnt) + "\":");
        inputStr = inputStr.substr(findSplit + 1);
        findSplit = inputStr.find("|");
    }
    if(last)
        resultStr += (inputStr + "\n\t}\n");
    else
        resultStr += (inputStr + "\n\t},\n");
    inputStr = resultStr;
}

void cutPartialInput(int startLine, int endLine, int threadIndex, bool last = false) {
    // convert from input's startLine to (endLine - 1)
    int startTime = clock();
    for(int i = startLine; i < endLine; i++) {
        convertStr(inputVec.at(i), (i == endLine - 1 && last ? true : false));
    }
    int endTime = clock();
    // if(threadIndex != 0)
        // cout << "> Thread " << threadIndex << " finished converting with " << (endTime - startTime) / double(CLOCKS_PER_SEC) << " seconds." << '\n';
}

void convertAllInputWithThread(int fileSize, int threadNum, int threadSize, int leftSize, bool last = false) {
    vector<thread> threadVec;
    int startLine = 0;
    int endLine = threadSize;
    for(int i = 0; i < threadNum; i++) {
        // cout << "> Program starts converting using thread " << i + 1 << "...\n";
        threadVec.emplace_back(cutPartialInput, startLine, endLine, i + 1, ((leftSize == 0 && i == threadNum - 1 && last) ? true : false));
        startLine += threadSize;
        endLine += threadSize;
    }
    threadVec.emplace_back(cutPartialInput, fileSize - leftSize, fileSize, 0, (leftSize != 0 && last ? true : false));
    for(auto& t: threadVec) {
        t.join();
    }
}

int main(int argc, char *argv[]) {
    // get thread number
    int threadNum = (argc > 1) ? atoi(argv[1]) : 3;

    // start the clock
    int startTime = clock();

    // file I/O
    ifstream fin;
    fin.open("input.csv");
    ofstream fout;
    fout.open("output.json");

    // max size of input is 4000000
    int inputSize = 2000000;
    inputVec.resize(inputSize);

    // check if buffer has overflow
    bool bufferFull = false;
    bool firstTimeBufferFull = true;
    bool coutText = true;

    // get input
    int index = 0;
    int inputStart = clock();
    while(!fin.eof()) {
        if(coutText) {
            cout << "> \n";
            cout << "> Program starts reading \'input.csv\'...\n";
            coutText = false;
        }
        string getl;
        fin >> getl;
        inputVec.at(index) = getl;
        if(index == inputSize - 1) {
            coutText = true;
            int inputEnd = clock();
            cout << "> Program finished reading part of \'input.csv\' with " << (inputEnd - inputStart) / double(CLOCKS_PER_SEC) << " seconds.\n";
            cout << "> Program starts converting \'input.csv\' with following information :\n";
            // information
            cout << "> /**********************************/\n";
            cout << "> /* Buffer size : " << inputSize << " lines\n";
            cout << "> /* Threads count : " << threadNum << " threads\n";
            cout << "> /* Thread size : " << inputSize / threadNum << " lines\n";
            cout << "> /* Left lines : " << inputSize % threadNum << " lines\n";
            cout << "> /**********************************/\n";

            bufferFull = true;
            // reach EOF
            // convert
            int convertStart = clock();
            convertAllInputWithThread(inputSize, threadNum, inputSize / threadNum, inputSize % threadNum);
            int convertEnd = clock();
            cout << "> Program finished converting part of \'input.csv\' with " << (convertEnd - convertStart) / double(CLOCKS_PER_SEC) << " seconds\n";
            // output
            cout << "> Program starts outputting part of converted \'input.csv\' to \'output.json\'...\n";
            int outputStart = clock();
            if(firstTimeBufferFull) {
                fout << "[\n";
                firstTimeBufferFull = false;
            }
            for(auto& i: inputVec)
                fout << i;
            int outputEnd = clock();
            cout << "> Program finished outputting part of converted \'input.csv\' to \'output.json\' with " << (outputEnd - outputStart) / double(CLOCKS_PER_SEC) << " seconds\n";
            cout << "> \n";
            index = 0;
            inputStart = clock();
        }
        else {
            index++;
        }
    }
    // cout << index << endl;
    if(index != 0) {
        int inputEnd = clock();
        cout << "> Program finished reading part of \'input.csv\' with " << (inputEnd - inputStart) / double(CLOCKS_PER_SEC) << " seconds.\n";
        cout << "> Program starts converting \'input.csv\' with following information :\n";
        inputVec.erase(inputVec.begin() + index, inputVec.end());
        // information
        cout << "> /**********************************/\n";
        cout << "> /* Buffer size : " << inputVec.size() << " lines\n";
        cout << "> /* Threads count : " << threadNum << " threads\n";
        cout << "> /* Thread size : " << inputVec.size() / threadNum << " lines\n";
        cout << "> /* Left lines : " << inputVec.size() % threadNum << " lines\n";
        cout << "> /**********************************/\n";

        // convert
        int convertStart = clock();
        convertAllInputWithThread(inputVec.size(), threadNum, inputVec.size() / threadNum, inputVec.size() % threadNum, true);
        int convertEnd = clock();
        cout << "> Program finished converting part of \'input.csv\' with " << (convertEnd - convertStart) / double(CLOCKS_PER_SEC) << " seconds\n";
        // output
        cout << "> Program starts outputting part of converted \'input.csv\' to \'output.json\'...\n";
        int outputStart = clock();
        if(!bufferFull)
            fout << "[\n";
        for(auto& i: inputVec)
            fout << i;
        int outputEnd = clock();
        cout << "> Program finished outputting part of converted \'input.csv\' to \'output.json\' with " << (outputEnd - outputStart) / double(CLOCKS_PER_SEC) << " seconds\n";
    }
    fout << "]";
    int endTime = clock();
    cout << "> Program finished with total time : " << (endTime - startTime) / double(CLOCKS_PER_SEC) << " seconds" << '\n';

    return 0;
}
