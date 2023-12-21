#include <iostream>
#include <cmath>
#include "tiny.cpp"

using namespace std;

void insertInSymbolTable(TreeNode* node, SymbolTable* symbol_table){
    if (node->node_kind == READ_NODE || node->node_kind == ASSIGN_NODE || node->node_kind == ID_NODE ){
        symbol_table->Insert(node->id, node->line_num); //base case
    }
}

void getSyntaxTree(TreeNode* node, SymbolTable* symbol_table){
    insertInSymbolTable(node,symbol_table);

    for(int i=0;i<MAX_CHILDREN;i++){
        if(node->child[i]){
            getSyntaxTree(node->child[i], symbol_table); //check children nodes recursively
        }
    }

    //check and assign data types (int , bool)
    if(node->node_kind == OPER_NODE){
        if(node->oper == LESS_THAN ||  node->oper == EQUAL ){
            node->expr_data_type = BOOLEAN;
        }
        else {
            node->expr_data_type = INTEGER;
        }
    }

    else if( node->node_kind == NUM_NODE || node->node_kind==ID_NODE ){
        node->expr_data_type=INTEGER;
    }

    if(node->sibling) {
        getSyntaxTree(node->sibling, symbol_table); //check sibling / subtree
    }
}


int getOperation(TreeNode* node, SymbolTable* symbol_table, int*variables){

    if(node->node_kind == ID_NODE){
        return variables[symbol_table->Find(node->id)->memloc];// return the value of variable in RHS
    }
    if(node->node_kind == NUM_NODE){
        return node->num; // return node number
    }

    int x = getOperation(node->child[0], symbol_table, variables);
    int y = getOperation(node->child[1], symbol_table, variables);

    switch (node->oper) {
        case EQUAL:
            return x == y;
        case LESS_THAN:
            return x<y;
        case PLUS:
            return x+y;
        case MINUS:
            return x-y;
        case TIMES:
            return x*y;
        case DIVIDE:
            return x/y;
        case POWER:
            return pow(x,y);
        default:
            return 0;

    }
}

void runSimulation(TreeNode* node, SymbolTable* symbol_table, int*variables){

    switch (node->node_kind) {
        case READ_NODE: {
            cout << "\nEnter " << node->id << ": \n";
            int num;
            cin >> num;
            variables[symbol_table->Find(node->id)->memloc] = num;
            break;
        }
        case WRITE_NODE: {
            int num = getOperation(node->child[0], symbol_table, variables);
            cout << "\nval : " << num << endl;
            break;
        }
        case ASSIGN_NODE: {
            int num = getOperation(node->child[0], symbol_table, variables);
            variables[symbol_table->Find(node->id)->memloc] = num;
            break;
        }
        case IF_NODE: {
            bool condition = getOperation(node->child[0], symbol_table, variables);
            if (condition) {
                runSimulation(node->child[1], symbol_table, variables);
            } else if (node->child[2]) {
                runSimulation(node->child[2], symbol_table, variables);
            }
            break;
        }
        case REPEAT_NODE: {
            do {
                runSimulation(node->child[0], symbol_table, variables);
            } while (!getOperation(node->child[1], symbol_table, variables));
            break;
        }
    }

    if(node->sibling){ // check  sibling node if found
        runSimulation(node->sibling, symbol_table, variables);
    }

}


int main(){

    CompilerInfo Tiny("D:\\7th Semester\\Compilers\\assign4\\input.txt", "output.txt", "debug.txt");
    TreeNode* syntax_tree = Parse(&Tiny);
    SymbolTable symbol_table;
    getSyntaxTree(syntax_tree, &symbol_table);
    cout<<"1) Symbol Table :- "<<endl;
    symbol_table.Print();
    cout<<"----------------------"<<endl;

    cout<<"2) Syntax Tree :- "<<endl;
    PrintTree(syntax_tree);
    cout<<"----------------------"<<endl;


    cout<<"3) Run Simulation :-"<<endl;
    int* variables=new int[symbol_table.num_vars];
    for(int i=0;i<symbol_table.num_vars;i++){
        variables[i] = 0;
    }
    runSimulation(syntax_tree, &symbol_table, variables);
    cout<<"----------------------"<<endl;

    symbol_table.Destroy();
    DestroyTree(syntax_tree);
    return 0;
}