**How I implement BWT encoding?**

Firstly, I used struct to store suffix array and its starting position in original string. To sort the suffix array, qsort() is used and as for compare function, not only ascii code of the string are compared but also the position of delimiter counts. i.e. if two string has the same prefix includes a delimiter at the same position, the one starts with small index will be considered smaller than the other one. 

According to sorted suffix array, we get bwt by suffix array starting positon index subtract one(if -1 is obtained, then last element should be pointed to). At the same time, records delimiter postions in bwt order, for the original string delimiter order is ascending comparing to the bwt order we can easily know how delimiter are sorted and index order is transfered to id order, which is stored in auxiliary positional information file. 
  
**How I operate BWT backward search?**
 
For search, first we sort the encoded bwt string to get lexicographically  sorted first column. I used two 1d array to store C-table, which is crucial to backward search. Calculate first and last index by formula with select and occurence. If last index is ahead of first after loop, it means no records was found. If not, last - first + 1 gives the number of found searched pattern, which is required in m search.

And for n search, we need to know the matching bwtween delimiters and records id. And by applying lf mapping, we know the character and its index before each delimiter.
 
To get number of search records, we need lp mapping, which is calculated by formula.  

**Approach to improve the algorithm in time complexity**
To improve the time complexity when it comes to large files like 50G or so, the auxiliary file records counts of characters appearing before a position in a flexible length interval(changes according to the file storage), which helps with position calculation of the lp mapping. 
