# DDA_Reader
Ducati DDA file reader

I did this little program for be able to extract Ducati DDA files info into XML.
The idea is to collect these data for statistics purpose or virtual dashboard (for video).
I used the XML format because you can easily convert it into any other format.

I tried to do a generic code using an XML definition file. 
Since the DDA format is evolving constantly, you can add a new version into the XML.

The tricky part is to find the header size and the parameters definition order, size, etc...
But it is pretty simple if you can compare your results with the real Ducati tool.
TIPS: Start with the distance (your bike mileage) it is an easy one to catch.

I tested it on my own DDA run coming from my 2014 Ducati Streetfighter 848.
The DDA itself is a used one I bought on ebay (apparently coming from a 1198SBK).
Some info about the 1198SBK are still in the DDA files headers (Not sure why).
The DDA version is 4.
