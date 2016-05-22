/*
PageRank(d, diffPR, maxIterations)

            Read "web pages" from the collection in file "collection.txt"
            and build a graph structure using Adjacency List Representation

            N = number of urls in the collection
            diff = 0;
            For each url pi in the collection
                PR(pi) = 1/N  ;

            iteration = 0;
            While (iteration < maxIteration AND diff >= diffPR)
                iteration++;
                For each url pi in the collection
                    PR_old = PR(pi);
                    sum = 0 ;
                    for each url pj pointing to pi (ignore self-loops and parallel edges)
                        sum = sum + PR(pj) / out-degree-of(pj);

                    PR(pi) = (1-d)/N + d * sum
                    diff = diff + Abs(PR_old - PR(pi);
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "graph.h"
#include "DLList.h"
#include "url.h"

#define MAXSTRING 50
#define BUFSIZE 50

char* stradd(const char* a, const char* b);


int main (int argc, const char * argv[]) {

    int i;
    DLList URLs = newDLList();
    char buffer[BUFSIZE];
    int URLcount = 0;
    FILE * collection = fopen ("collection.txt", "r");
    while (!feof(collection)) {
        while ((fgets(buffer, sizeof(buffer), collection) != NULL)) {
            char *cur, link[BUFSIZE];
            cur = buffer;
            while ((cur = nextURL(cur)) != NULL) {

                getURL(cur, link, BUFSIZE - 1);
                DLListAfter(URLs, link, 0);
                //showDLList(URLs);
                URLcount++;
                printf("%s, %d\n", link, URLcount);
                cur += strlen(link);
            }
        }
    }
    fclose(collection);
    int p = DLListLength(URLs);
    /* FOR TESTING
    printf("length of list: %d\n", p);
    showDLList(URLs);
    DLListMoveTo(URLs, 1);
    //char *curr = DLListCurrent(URLs);
    //printf("%s\n", curr);
    */
    double initPageRank = 1 / (float)p;
    printf("initPageRank: %f\n", initPageRank); //FOR TESTING
    DLListMoveTo(URLs, 1);
    for (i = 0; i <= URLcount; i++) {

        alterPageRank (URLs, initPageRank);
        DLListMove(URLs, 1);
    }
    showDLList(URLs); //FOR TESTING
    Graph webPages = newGraph(URLcount);
    for (i = 0; i < URLcount; i++) {
        DLListMoveTo(URLs, i + 1);
        char * outgoingURL = DLListCurrent(URLs);
        char * outgoingURLtext = stradd(outgoingURL, ".txt");
        //printf("%s\n", outgoingURL); //FOR TESTING
        FILE *f = fopen(outgoingURLtext, "r");
        while ((fgets(buffer, sizeof(buffer), f) != NULL)) {
            char *cur, link[BUFSIZE];
            cur = buffer;
            while ((cur = nextURL(cur)) != NULL) {
                getURL(cur, link, BUFSIZE - 1);
                //printf("%s\n", link);
                addEdge(webPages, outgoingURL, link);
                cur += strlen(link);
//create linked list with outgoing URLs with it's pagerank divided by number of outgoing URLs
            }
        }
        fclose(f);
    }
//********** currently working on (all above works) **************
    int iteration;
    int maxIterations = 100;
    //double diffPR = 0.00001;
    //double diff = 0;
    char * checkingFrom;
    char * checkingTo;
    int connections = 0;
    int j = 0;
    double d = 0.85;
    double initPageRank2[URLcount];
    for (iteration = 0; iteration < maxIterations /*&&& diff >= diffPR*/; iteration ++) {
        //DLListMoveTo(URLs, 1);
        for (i = 0; i <= URLcount; i++) {
            initPageRank2[i] = getPageRank(URLs);
            DLListMoveTo(URLs, i + 1);
        }
        //diff = 0;
        //showGraph(webPages, 0); //FOR TESTING
        for (i = 0; i <= URLcount; i++) {
            connections = 0;
            DLListMoveTo(URLs, i + 1);
            checkingFrom = DLListCurrent(URLs);
            j = 0;
            for (j = 0; j <= URLcount; j++) {
                DLListMoveTo(URLs, j + 1);
                checkingTo = DLListCurrent(URLs);
                //check all out going links from checkingFrom, sum them, then add that value to pr of the checking to
                connections = connections + isConnected(webPages, checkingFrom, checkingTo);
            }
            //printf("isconnected?? %d\n", connections); //FOR TESTING
            if (connections > 0) {
                DLListMoveTo(URLs, i + 1);
                double originalPrfrom = getPageRank(URLs);
                //printf("originalPrfrom: %lf\n", originalPrfrom ); //FOR TESTING
                for (j = 0; j <= URLcount; j++) {
                    DLListMoveTo(URLs, j + 1);
                    checkingTo = DLListCurrent(URLs);
                    //check all out going links from checkingFrom, sum them, then add that value to pr of the checking to
                    if (isConnected(webPages, checkingFrom, checkingTo)) {
                        double originalPrTo = getPageRank(URLs);
                        if (originalPrTo == initPageRank2[i]) {
                            alterPageRank (URLs, ((double)originalPrfrom / (double)connections));
                        } else {
                            //printf("name: %s, old pr: %lf\n", checkingTo, originalPrTo); //FOR TESTING
                            alterPageRank (URLs, ((double)originalPrTo + ((double)originalPrfrom / (double)connections)));
                        }
                        //double prToPrint = getPageRank(URLs); //FOR TESTING
                        //printf("name: %s, new pr: %lf\n", checkingTo, prToPrint); //FOR TESTING
                        //diff = diff + fabs(originalPrTo - getPageRank(URLs));
                    }
                }
            }
        }
        showDLList(URLs);
        //DLListMoveTo(URLs, 1);
        for (i = 0; i <= URLcount; i++) {
            DLListMoveTo(URLs, i + 1);
            double originalPr = getPageRank(URLs);
            alterPageRank(URLs, (((1 - d) / URLcount) + (d * originalPr)));
        }
    }
    showDLList(URLs);
    //showGraph(webPages, 0);
    return EXIT_SUCCESS;
}



char* stradd(const char* a, const char* b) {
    size_t len = strlen(a) + strlen(b);
    char *ret = (char*)malloc(len * sizeof(char) + 1);
    *ret = '\0';
    return strcat(strcat(ret, a) , b);
}


