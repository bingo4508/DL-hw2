args <- commandArgs(trailingOnly = TRUE)
t <- read.table(args, sep=" ", row.names=1)
t <- scale(t)
write.table(t, paste(args,"normalize",sep="_"), sep=" ", row.names=TRUE, col.names=FALSE, quote=FALSE)
