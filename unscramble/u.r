library(seriation)
library(imager)
library(purrr)

im <- load.image("https://camo.githubusercontent.com/7d413e9a343e8ceaa507d68a1a6e93247d5f7853/68747470733a2f2f726f62696e686f7573746f6e2e6769746875622e696f2f696d6167652d756e736872656464696e672f696d616765732f6f726967696e616c2f626c75652d686f75722d70617269732e706e67")

scramble <- function(im,axis="x")
{
    imsplit(im,axis) %>% { .[sample(length(.))] } %>% imappend(axis) 
}


unscramble <- function(im.s,axis="x",method="TSP",...)
{
    cols <- imsplit(im.s,axis)
    #Compute a distance matrix (using L1 - Manhattan - distance)
    #Each entry D_ij compares column i to column j  
    D <- map(cols,as.vector) %>% do.call(rbind,.) %>% dist(method="manhattan")
    out <- seriate(D,method=method,...)
    cols[get_order(out)] %>% imappend(axis) 
}

im.s <- scramble(im,"x") %>% scramble("y")

#The double scramble produces an unrecognisable mess
jpeg('rplot.jpg')
plot(im.s)
dev.off()

jpeg('uns.jpg')
unscramble(im.s,"y") %>% unscramble("x") %>% plot
dev.off()

