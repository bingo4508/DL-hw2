#include<stdlib.h>
#include<stdio.h>

void outputResult(char *before, char *afterTrim)
{
	FILE *beforeTrim = fopen(before, "r");
        FILE *outFp = fopen(afterTrim, "w");
        fprintf(outFp, "id,phone_sequence\n");
        rewind(beforeTrim);
        
        char id[40];
        int phonmeIdx;
        int frameSize;
        int isSilHead;
        int prePhmIdx;
        int i;
        while(fscanf(beforeTrim, "%s ", id) != EOF)
        {
                fprintf(outFp, "%s,", id);
                fscanf(beforeTrim, "%d", &frameSize);
                isSilHead = 1;
                prePhmIdx = -1;
                for (i = 0; i < frameSize; i++)
                {
                        fscanf(beforeTrim, "%d", &phonmeIdx);
                        if (isSilHead && phonmeIdx != 37)       //first not-sil phonme
                                isSilHead = 0;
                        if (!isSilHead && prePhmIdx != phonmeIdx)
                        {
                                if (phonmeIdx != 37)
                                {
                                        if (prePhmIdx == 37)
                                                fprintf(outFp, "L");
                                        if (phonmeIdx < 26)
                                                fprintf(outFp, "%c", (char)(phonmeIdx+'a'));
                                        else
                                                fprintf(outFp, "%c", (char)(phonmeIdx-26+'A'));
                                }
                                prePhmIdx = phonmeIdx;
                        }
                }
                fprintf(outFp, "\n");
        }
        fclose(outFp);
}

int main(int argc, char *argv[])
{
	if(argc != 3){
		printf("args: $output $answer\n");
	}else{
		outputResult(argv[1], argv[2]);
	}

	return 0;
}
