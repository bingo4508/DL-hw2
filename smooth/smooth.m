function smooth(inputFileName,outputFileName)
%inputFileName='smooth_test';
%outputFileName='smooth_test_output';
if nargin<1, selfdemo; return; end
if nargin<2, inputFileName='smooth_test'; end
if nargin<3, outputFileName='smooth_test_output'; end
fid = fopen(inputFileName);
fid2 = fopen(outputFileName,'wt');
tline = fgets(fid);
while ischar(tline)
    readed=strread(tline,'%s','delimiter',' ');
    name=readed{1};
    Vector=zeros(1,length(readed)-2);
    for i=2:length(readed)-1
    Vector(i-1)=str2num(readed{i});
    end
    for i=1+2:length(Vector)-2
        if Vector(i-2)==Vector(i-1)&&Vector(i-1)==Vector(i+1)&&Vector(i+1)==Vector(i+2)
            Vector(i)=Vector(i-1);
        end
    end
    for i=1+3:length(Vector)-3
        if Vector(i-3)==Vector(i-2)&&Vector(i-2)==Vector(i-1)&&Vector(i+1)==Vector(i+2)&&Vector(i+2)==Vector(i+3)&&Vector(i)~=Vector(i-1)&&Vector(i)~=Vector(i+1);
            Vector(i)=Vector(i-1);
        end
    end
    fprintf(fid2,name);
    fprintf(fid2,' ');
    for i=1:length(Vector)
        fprintf(fid2,'%d',Vector(i));
        fprintf(fid2,' ');
    end
    fprintf(fid2,'\n');
    tline = fgets(fid);
end
fclose(fid);
fclose(fid2);

function selfdemo
inputFileName='smooth_test';
outputFileName='smooth_test_output';
smooth(inputFileName,outputFileName)
