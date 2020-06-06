#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <exception>
#include <LuceneHeaders.h>
#include <ConcurrentMergeScheduler.h>
#include <FileUtils.h>
#include <MiscUtils.h>

extern "C" 
{
    #include <string.h>
    #include <sys/time.h>
}

#define NUM_THREADS 8

using namespace std;
using namespace Lucene;

void parallel_index(int tid, int num_threads, IndexWriterPtr iwriter, 
        vector<DocumentPtr> documents)
{
    for (tid; tid < documents.size(); tid += num_threads) {
        iwriter->addDocument(documents[tid]);
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
    ifstream in;
    vector<string> inputFiles;
    vector<DocumentPtr> documents;
    struct timeval start, end;
    long indexTime, indexSize, searchTime;
    ConcurrentMergeSchedulerPtr scheduler;
    vector<thread> threads;
    
    indexTime = 0;
    indexSize = 0;
    searchTime = 0;
    try {
        gettimeofday(&start, NULL);
        inputFiles = vector<string>();

        in.open(argv[1], ifstream::in);
        while(in.good()) {
            char cline[2048];
            in.getline(cline, 2048);
            if (strlen(cline) <= 0) {
                break;
            }
            inputFiles.push_back(string(cline));
        }
        in.close();

        documents = vector<DocumentPtr>();
        for (auto inputFile : inputFiles) {
            DocumentPtr document = newLucene<Document>();
            char line[2048], *tok, content[2048], filepath[2048];
            int i = 16;
            
            strcpy(line, inputFile.c_str());
            strcpy(content, "");
            strcpy(filepath, "");
            tok = strtok(line, " ");
            while (tok != NULL) {
                if (strcmp(filepath, "") != 0) {
                    strcat(filepath, " ");
                }

                if (i != 0) {
                    strcat(content, tok);
                    strcat(content, " ");
                    i--;
                } else {
                    strcat(filepath, tok);
                }

                tok = strtok(NULL, " ");
            }

            strcat(content, filepath);

            String sourceFile(StringUtils::toUnicode(filepath));
            String contentFile(StringUtils::toUnicode(content));

            document->add(newLucene<Field>(L"content", contentFile,
                    Field::STORE_NO, Field::INDEX_ANALYZED));
            document->add(newLucene<Field>(L"filepath", sourceFile,
                    Field::STORE_YES, Field::INDEX_NOT_ANALYZED));

            documents.push_back(document);
        }

        analyzer = newLucene<StandardAnalyzer>(LuceneVersion::LUCENE_CURRENT);
        directory = newLucene<RAMDirectory>();
        iwriter = newLucene<IndexWriter>(directory, analyzer, true,
                IndexWriter::MaxFieldLengthUNLIMITED);

        scheduler = boost::static_pointer_cast<ConcurrentMergeScheduler>(
                iwriter->getMergeScheduler());
        scheduler->setMaxThreadCount(8);
        iwriter->setMergeScheduler(scheduler);
        
        for (int tid = 0; tid < NUM_THREADS; tid++) {
            threads.push_back(thread(parallel_index, tid, NUM_THREADS, 
                    iwriter, documents));
        }
        for (auto& th : threads) {
            th.join();
        }
        iwriter->commit();
        gettimeofday(&end, NULL);
        indexTime += (((long) end.tv_sec - (long) start.tv_sec) 
                * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;

        indexSize = directory->sizeInBytes() / 1000;
        iwriter->close();

        gettimeofday(&start, NULL);
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
                * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;
    } catch(LuceneException& e) {
        wcout << e.getError() << endl;
    } catch(std::runtime_error& e) {
        cout << e.what() << endl;
    }

    cout << "IndexTime: " << indexTime << " ms" << endl;
    cout << "IndexSize: " << indexSize << " kB" << endl;
    cout << "SearchTime: " << searchTime << " ms" << endl;

    return 0;
}
