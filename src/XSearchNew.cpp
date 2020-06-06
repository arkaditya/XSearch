#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <exception>
#include <LuceneHeaders.h>
#include <ConcurrentMergeScheduler.h>
#include <DocumentsWriter.h>
#include <FileUtils.h>
#include <MiscUtils.h>
#include <vector>

extern "C" 
{
    #include <string.h>
    #include <sys/time.h>
}

//#define NUM_THREADS 8

using namespace std;
using namespace Lucene;

void parallel_index(IndexWriterPtr iwriter, vector<string> inputFiles, int startPos,int endPos)
{
    //documents = vector<DocumentsPtr>();

    for (int i = startPos ; i <= endPos ; i++) {
            DocumentPtr document = newLucene<Document>();
            char filename[1024];
            strcpy(filename, inputFiles[i].c_str());
            String sourceFile(StringUtils::toUnicode(filename));

            document->add(newLucene<Field>(L"content",
                    newLucene<FileReader>(sourceFile)));
            document->add(newLucene<Field>(L"filepath", sourceFile,
                    Field::STORE_YES, Field::INDEX_NOT_ANALYZED));

            //documents.push_back(document);
    	    iwriter->addDocument(document);
        }

}

int main(int argc, char **argv)
{
    AnalyzerPtr analyzer;
    RAMDirectoryPtr directory;
    IndexWriterPtr iwriter;
    IndexReaderPtr ireader;
    IndexSearcherPtr isearcher;
    QueryParserPtr parser;
    DocumentsWriterPtr docWriter;
    ifstream in;
    vector<string> inputFiles;
    struct timeval start, end;
    long indexTime, indexSize, searchTime;
    ConcurrentMergeSchedulerPtr scheduler;
    vector<thread> threads;
    int startPos,endPos;
    int numThreads;
    int fileThreadRatio,noFiles,RAMBuffer,BufferedDocs;

    numThreads = atoi(argv[2]);
    RAMBuffer = atoi(argv[4]);
    BufferedDocs = atoi(argv[3]);
    indexTime = 0;
    indexSize = 0;
    searchTime = 0;
    try {
        inputFiles = vector<string>();

        in.open(argv[1], ifstream::in);
        while(in.good()) {
            char cline[1024];
            in.getline(cline, 1024);
            if (strlen(cline) <= 0) {
                break;
            }
            inputFiles.push_back(string(cline));
        }
        in.close();
       
	noFiles = inputFiles.size();
	 
        fileThreadRatio = (int)( inputFiles.size() / numThreads );

        gettimeofday(&start, NULL);
        analyzer = newLucene<StandardAnalyzer>(LuceneVersion::LUCENE_CURRENT);
        directory = newLucene<RAMDirectory>();
        iwriter = newLucene<IndexWriter>(directory, analyzer, true,
                IndexWriter::MaxFieldLengthUNLIMITED);
	
	iwriter->setMaxBufferedDocs(BufferedDocs);
	iwriter->setRAMBufferSizeMB(RAMBuffer);
        scheduler = boost::static_pointer_cast<ConcurrentMergeScheduler>(
                iwriter->getMergeScheduler());
        scheduler->setMaxThreadCount(10);
        iwriter->setMergeScheduler(scheduler);
        
        startPos = 0;
        endPos = fileThreadRatio - 1;
        for (int tid = 0; tid < numThreads; tid++) {
           
	    threads.push_back(thread(parallel_index, iwriter, inputFiles, startPos,endPos));
            
	    startPos = startPos + fileThreadRatio;
            
            if (tid != numThreads - 1 ){
                    endPos = startPos + fileThreadRatio -1;
            } else {
                    endPos = inputFiles.size() - 1;
            }

        }

        for (auto& th : threads) {
            th.join();
        }

        iwriter->commit();
	iwriter->optimize();

        indexSize = directory->sizeInBytes() / 1000;
        iwriter->close();
        
	gettimeofday(&end, NULL);
        indexTime += (((long) end.tv_sec - (long) start.tv_sec) 
                * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;

  /*      gettimeofday(&start, NULL);
        ireader = IndexReader::open(directory);
        isearcher = newLucene<IndexSearcher>(ireader);
        parser = newLucene<QueryParser>(LuceneVersion::LUCENE_CURRENT,
                L"content", analyzer);

        in.open(argv[2], ifstream::in);
        while(in.good()) {
            char word[1024];

            in.getline(word, 1024);
            if (strlen(word) <= 0) {
                break;
            }
            
            String term(StringUtils::toUnicode(word));
            QueryPtr query = parser->parse(term);
            
            Collection<ScoreDocPtr> hits = isearcher->search(
                    query, 1000)->scoreDocs;
        }
        in.close();

        isearcher->close();
        directory->close();
        gettimeofday(&end, NULL);
        searchTime += (((long) end.tv_sec - (long) start.tv_sec) 
                * 1000000 + (end.tv_usec - start.tv_usec)) / 1000; */
	inputFiles.clear();
	threads.clear();
    } catch(LuceneException& e) {
        wcout << e.getError() << endl;
    } catch(std::runtime_error& e) {
        cout << e.what() << endl;
    }
    //-----------------------Output ---------------------
    cout << "0" << " "  << indexTime  << " " << "0" << " " << BufferedDocs << " " << RAMBuffer << " "  << numThreads << " " <<  fileThreadRatio << endl;
    //cout << "SearchTime: " << searchTime << " ms" << endl;

    return 0;
}

