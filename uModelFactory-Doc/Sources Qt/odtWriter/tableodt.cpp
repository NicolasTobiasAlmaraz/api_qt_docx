#include <tableodt.h>

const int tableOdt::APPEND = 0;
const int tableOdt::INDEX_ERROR = 1;
const int tableOdt::REPLACE = 2;

tableOdt::tableOdt(int rows, int columns) {
    mRows = rows;
    mColumns = columns;
}

int tableOdt::addElement(tableElement item) {
    int size = listElements.size();
    int row=item.getRow();
    int col=item.getColumn();
    int ret = APPEND;

    if(col>mColumns || row>mRows)
        return INDEX_ERROR;

    bool fAppend = true;
    for(int i=0 ; i<size ; i++) {
        //Tomo elemento
        tableElement aux = listElements.front();
        listElements.pop_front();
        if(col == aux.getColumn() && row == aux.getRow() ) {
            //Me fijo si debo reemplazarlo por el nuevo item
            listElements.push_back(item);
            fAppend = false;
            ret = REPLACE;
        } else
            //Sino lo vuelvo a meter
            listElements.push_back(aux);
    }
    //Si no reemplace ninguno entonces lo agrego al final
    if(fAppend)
        listElements.push_back(item);
    return ret;
}
