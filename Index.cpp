#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int MAXW = 50;
const int MAXP = 50;

int loadInventory(const string& filename, string products[], string warehouses[], int inv[][MAXP], int& W, int& P);
void processTransactions(const string& transfile, string products[], string warehouses[], int inv[][MAXP], int W, int P);
int productIndex(const string products[], int P, const string& product);
int warehouseIndex(const string warehouses[], int W, const string& wh);
void writeInventory(const string& filename, string products[], string warehouses[], int inv[][MAXP], int W, int P);
void writeLowStockReport(const string& filename, string products[], string warehouses[], int inv[][MAXP], int W, int P, int threshold);

int main() {
    cout << "Program started\n";

    string products[MAXP], warehouses[MAXW];
    int inv[MAXW][MAXP];
    int W, P;

    if (!loadInventory("inventory.txt", products, warehouses, inv, W, P)) {
        cout << "File error\n";
        return 1;
    }

    processTransactions("transactions.txt", products, warehouses, inv, W, P);

    int threshold;
    cout << "Enter threshold: ";
    cin >> threshold;

    writeLowStockReport("lowstock.txt", products, warehouses, inv, W, P, threshold);
    writeInventory("inventory_updated.txt", products, warehouses, inv, W, P);
    cout << "Press Enter to exit...";
    cin.ignore();
    cin.get();
    return 0;

}

int loadInventory(const string& inventory, string products[], string warehouses[], int inv[][MAXP], int& W, int& P) {
    ifstream fin(inventory);
    if (!fin) return 0;

    fin >> W >> P;

    for (int j = 0; j < P; j++) fin >> products[j];

    for (int i = 0; i < W; i++) {
        fin >> warehouses[i];
        for (int j = 0; j < P; j++) fin >> inv[i][j];
    }

    fin.close();
    return 1;
}

int productIndex(const string products[], int P, const string& product) {
    for (int i = 0; i < P; i++)
        if (products[i] == product) return i;
    return -1;
}

int warehouseIndex(const string warehouses[], int W, const string& wh) {
    for (int i = 0; i < W; i++)
        if (warehouses[i] == wh) return i;
    return -1;
}

void processTransactions(const string& transfile, string products[], string warehouses[], int inv[][MAXP], int W, int P) {
    ifstream fin(transfile);
    if (!fin) {
        cout << "Transaction file error\n";
        return;
    }

    ofstream err("errors.txt");
    string type;

    while (fin >> type) {
        if (type == "SELL") {
            string wh, product;
            int qty;
            fin >> wh >> product >> qty;

            int wi = warehouseIndex(warehouses, W, wh);
            int pi = productIndex(products, P, product);

            if (wi != -1 && pi != -1) {
                if (inv[wi][pi] < qty) {
                    cout << "Error: Not enough stock\n";
                    err << "SELL " << wh << " " << product << " failed\n";
                }
                else inv[wi][pi] -= qty;
            }
        }

        else if (type == "RESTOCK") {
            string wh, product;
            int qty;
            fin >> wh >> product >> qty;

            int wi = warehouseIndex(warehouses, W, wh);
            int pi = productIndex(products, P, product);

            if (wi != -1 && pi != -1) inv[wi][pi] += qty;
        }

        else if (type == "TRANSFER") {
            string fromW, toW, product;
            int qty;
            fin >> fromW >> toW >> product >> qty;

            int f = warehouseIndex(warehouses, W, fromW);
            int t = warehouseIndex(warehouses, W, toW);
            int p = productIndex(products, P, product);

            if (f != -1 && t != -1 && p != -1) {
                if (inv[f][p] < qty) {
                    cout << "Error: Not enough stock\n";
                    err << "TRANSFER " << fromW << " " << toW << " failed\n";
                }
                else {
                    inv[f][p] -= qty;
                    inv[t][p] += qty;
                }
            }
        }
    }

    fin.close();
    err.close();
}

void writeInventory(const string&inventory_updated, string products[], string warehouses[], int inv[][MAXP], int W, int P) {
    ofstream fout(inventory_updated);

    fout << W << " " << P << endl;

    for (int j = 0; j < P; j++) fout << products[j] << " ";
    fout << endl;

    for (int i = 0; i < W; i++) {
        fout << warehouses[i] << " ";


        for (int j = 0; j < P; j++) fout << inv[i][j] << " ";
        fout << endl;
    }

    fout.close();
}

void writeLowStockReport(const string&lowstock, string products[], string warehouses[], int inv[][MAXP], int W, int P, int threshold) {
    ofstream fout(lowstock
    );

    for (int i = 0; i < W; i++) {
        for (int j = 0; j < P; j++) {
            if (inv[i][j] < threshold) {
                fout << warehouses[i] << " " << products[j] << " " << inv[i][j] << endl;
            }
        }
    }

    fout.close();
}