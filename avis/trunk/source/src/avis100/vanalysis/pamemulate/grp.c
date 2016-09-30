struct tagGROUPHEADER {
    char  cIdentifier[4];
    WORD  wCheckSum;
    WORD  cbGroup;
    WORD  nCmdShow;
    RECT  rcNormal;
    POINT ptMin;
    WORD  pName;
    WORD  wLogPixelsX;
    WORD  wLogPixelsY;
    BYTE  bBitsPerPixel;
    BYTE  bPlanes;
    WORD  wReserved;  // Should be 0x0000
    WORD  cItems;
    WORD  rgiItems[cItems];
}
