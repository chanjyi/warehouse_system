#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include <fstream>
using namespace std;

// Node for Stack and Queue
struct ItemNode {
    string itemName;
    ItemNode* next;
    int quantity;

    ItemNode(string name,int qty = 1) : itemName(name),quantity(qty), next(nullptr) {}
};

// Stack class for incoming items (LIFO)
class InventoryStack {
private:
    ItemNode* top;

public:

    ItemNode* getTop() const { return top; }

    void setTop(ItemNode* node) { top = node; }

    InventoryStack() : top(nullptr) {}

    ~InventoryStack() {
        while (!isEmpty()) {
            pop();
        }
    }

    bool isEmpty() const {
        return top == nullptr;
    }

    void push(string itemName, int qty = 1)
    {
        if (!isEmpty() && top->itemName == itemName)
        {
            top->quantity += qty;
            return;

        }
        ItemNode* newNode = new ItemNode(itemName, qty);;
        newNode->next = top;
        top = newNode;
    }

    string pop() {
    if (isEmpty()) {
        throw runtime_error("No items in inventory to process!");
    }

    ItemNode* temp = top;
    string name = temp->itemName;

    if (temp->quantity > 1) {
        temp->quantity--;
    } else {
        top = top->next;
        delete temp;
    }

    return name;}


    string peek() const {
         if (isEmpty()) {
            throw runtime_error("No incoming items.");
        }
        return top->itemName;
    }

    void displayAll() const {
    if (isEmpty()) {
        cout << "Inventory is empty." << endl;
        return;
    }
    cout << "Inventory items (top to bottom): ";
    ItemNode* current = top;
    while (current != nullptr) {
        cout << current->itemName <<  "(" << current->quantity << ") ";
        current = current->next;
    }
    cout << endl;
}

};

// Queue class for outgoing shipments (FIFO)
class ShippingQueue {
private:
    ItemNode* front;
    ItemNode* rear;

public:

    ItemNode* getFront() const { return front; }

    void setFront(ItemNode* node) { front = node; }

    ShippingQueue() : front(nullptr), rear(nullptr) {}

    ~ShippingQueue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    bool isEmpty() const {
        return front == nullptr;
    }

    void enqueue(string itemName,int qty = 1) {
    if (!isEmpty() && rear->itemName == itemName) {
        rear->quantity += qty;
        return;
    }
    ItemNode* newNode = new ItemNode(itemName, qty);
    if (isEmpty()) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
}

    string dequeue() {
        if (isEmpty()) {
            throw runtime_error("No items to ship.!");
        }

        ItemNode* temp = front;
        string name = temp->itemName;

        if (temp->quantity > 1) {
        temp->quantity--;
    } else {
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
    }

    return name;
    }


    string peek() const {
        if (isEmpty()) {
            throw runtime_error("No items in shipping queue.");
        }
        return front->itemName;
    }

    void displayAll() const {
    if (isEmpty()) {
        cout << "Shipping queue is empty." << endl;
        return;
    }
    cout << "Shipping queue items (front to rear): ";
    ItemNode* current = front;
    while (current != nullptr) {
        cout << current->itemName <<  "(" << current->quantity << ") ";
        current = current->next;
    }
    cout << endl;
}

};

// Warehouse System
class WarehouseSystem {
private:
    InventoryStack inventory;
    ShippingQueue shipping;

public:

    void loadFromFile(const string& filename) {
    ifstream infile(filename);

    if (!infile) {
        cout << "No previous save found. Starting fresh." << endl;
        return;
    }

    string line;
    bool loadingInventory = false;
    bool loadingShipping = false;

    while (getline(infile, line)) {
        if (line.find("--- Final Inventory ---") != string::npos) {
            loadingInventory = true;
            loadingShipping = false;
            continue;
        }
        if (line.find("--- Final Shipping Queue ---") != string::npos) {
            loadingInventory = false;
            loadingShipping = true;
            continue;
        }

        if (loadingInventory && line != "Inventory is empty.") {
            // Push onto stack (LIFO)
            inventory.push(line);
        }
        if (loadingShipping && line != "Shipping queue is empty.") {
            // Enqueue into queue (FIFO)
            shipping.enqueue(line);
        }
    }

    infile.close();
    cout << "Previous data loaded from " << filename << endl;
}

    void addItem(string itemName,int qty) {
        inventory.push(itemName,qty);
         cout << "Item \"" << itemName << "\" added to inventory. Quantity: " << qty << endl;
    }

    // Modified processItem to accept item name and quantity
    void processItem(const string& itemName, int qty) {
        try {
            // Find item in inventory
            ItemNode* cur = inventory.getTop();
            while (cur != nullptr) {
                if (cur->itemName == itemName) {
                    if (cur->quantity < qty) {
                        cout << "Not enough quantity in inventory to process!" << endl;
                        return;
                    }
                    cur->quantity -= qty;
                    if (cur->quantity == 0) {
                        // Remove node
                        inventory.setTop(cur->next);
                        delete cur;
                    }
                    shipping.enqueue(itemName, qty);
                    cout << "Processed \"" << itemName << "\" (" << qty << ") and added to shipping queue." << endl;
                    return;
                }
                cur = cur->next;
            }
            cout << "Item not found in inventory: " << itemName << endl;
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    // Modified shipItem to accept item name and quantity
    void shipItem(const string& itemName, int qty) {
        try {
            ItemNode* cur = shipping.getFront();
            ItemNode* prev = nullptr;
            while (cur != nullptr) {
                if (cur->itemName == itemName) {
                    if (cur->quantity < qty) {
                        cout << "Not enough quantity in shipping queue to ship!" << endl;
                        return;
                    }
                    cur->quantity -= qty;
                    cout << "Shipping item: " << itemName << " (" << qty << ")" << endl;
                    if (cur->quantity == 0) {
                        // Remove node
                        if (prev == nullptr) {
                            shipping.setFront(cur->next);
                        } else {
                            prev->next = cur->next;
                        }
                        delete cur;
                    }
                    return;
                }
                prev = cur;
                cur = cur->next;
            }
            cout << "Item not found in shipping queue: " << itemName << endl;
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    void viewLastIncoming() const {
         try {
            cout << "Last incoming item: " << inventory.peek() << endl;
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    void viewNextShipment() const {
        try {
            cout << "Next item to ship: " << shipping.peek() << endl;
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    void viewAll() const {
    cout << "\n--- Current Inventory ---" << endl;
    inventory.displayAll();
    cout << "--- Current Shipping Queue ---" << endl;
    shipping.displayAll();
}

// Search for an item in Inventory Stack and Shipping Queue
void searchItem(const string& name) const {
    try {
         // Search in Inventory (stack)
        ItemNode* cur = inventory.getTop();
        while (cur != nullptr) {
            if (cur->itemName == name) {
                cout << "Found in Inventory: " << name
                     << " (" << cur->quantity << ")" << endl;
                return;// Stop once found
            }
            cur = cur->next;// Move to next node
        }

        // Search in Shipping Queue
        cur = shipping.getFront();
        while (cur != nullptr) {
            if (cur->itemName == name) {
                cout << "Found in Shipping Queue: " << name
                     << " (" << cur->quantity << ")" << endl;
                return;// Stop once found
            }
            cur = cur->next;
        }

        // If not found in both structures, throw error
        throw runtime_error("Item not found: " + name);

    } catch (const runtime_error& e) {

        // Handle the error message here
        cout << e.what() << endl;
    }
}

// Remove an item by name (from Inventory or Shipping Queue)
void removeItem(const string& name) {
    try {

        // Search in Inventory (stack)
        ItemNode* cur = inventory.getTop();
        ItemNode* prev = nullptr;
        while (cur != nullptr) {
            if (cur->itemName == name) {
                if (cur->quantity > 1) {

                    // If quantity > 1, just decrease by 1
                    cur->quantity--;
                } else {

                    // If only one item, remove node completely
                    if (prev == nullptr) {

                        // Remove top node
                        inventory.setTop(cur->next);
                    } else {

                        // Bypass the current node
                        prev->next = cur->next;
                    }
                    delete cur;
                }
                cout << "Removed \"" << name << "\" from Inventory." << endl;
                return;
            }
            prev = cur;// Keep track of previous node
            cur = cur->next; // Move to next node
        }

        // Search in Shipping Queue
        cur = shipping.getFront();
        prev = nullptr;
        while (cur != nullptr) {
            if (cur->itemName == name) {
                if (cur->quantity > 1) {

                    // If quantity > 1, just decrease by 1
                    cur->quantity--;
                } else {

                     // If only one item, remove node completely
                    if (prev == nullptr) {

                         // Remove front node
                        shipping.setFront(cur->next);
                    } else {
                        prev->next = cur->next;
                    }
                    delete cur;
                }
                cout << "Removed \"" << name << "\" from Shipping Queue." << endl;
                return;
            }
            prev = cur;
            cur = cur->next;
        }

        throw runtime_error("Item not found: " + name);

    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }
}

// Count total items (Inventory + Shipping Queue)
void countItems() const {
    int count = 0;

    // Count items in Inventory (stack)
    ItemNode* cur = inventory.getTop();
    while (cur != nullptr) {
        count += cur->quantity;
        cur = cur->next;
    }

    // Count items in Shipping Queue
    cur = shipping.getFront();
    while (cur != nullptr) {
        count += cur->quantity;
        cur = cur->next;
    }
    cout << "Total items in system: " << count << endl;
}


void saveToFile(const string& filename) const {
    ofstream outfile(filename);

    if (!outfile) {
        cout << "Error: Could not open file for writing!" << endl;
        return;
    }

    outfile << "--- Final Inventory ---" << endl;
    if (inventory.isEmpty()) {
        outfile << "Inventory is empty." << endl;
    } else {
        ItemNode* current = inventory.getTop();  // need access
        while (current != nullptr) {
            outfile << current->itemName << endl;
            current = current->next;
        }
    };

    outfile << "--- Final Shipping Queue ---" << endl;
    if (shipping.isEmpty()) {
        outfile << "Shipping queue is empty." << endl;
    } else {
        ItemNode* current = shipping.getFront();  // need access
        while (current != nullptr) {
            outfile << current->itemName << endl;
            current = current->next;
        }
    }

    outfile.close();
    cout << "Final result saved to " << filename << endl;
}



    // Update an item's name or quantity in Inventory or Shipping Queue
    void updateItem(const string& oldName, const string& newName, int newQty) {
        bool found = false;
        // Update in Inventory
        ItemNode* cur = inventory.getTop();
        while (cur != nullptr) {
            if (cur->itemName == oldName) {
                cur->itemName = newName;
                cur->quantity = newQty;
                cout << "Updated item in Inventory: " << oldName << " -> " << newName
                     << " (Qty: " << newQty << ")" << endl;
                found = true;
                break;
            }
            cur = cur->next;
        }
        // Update in Shipping Queue
        cur = shipping.getFront();
        while (cur != nullptr) {
            if (cur->itemName == oldName) {
                cur->itemName = newName;
                cur->quantity = newQty;
                cout << "Updated item in Shipping Queue: " << oldName << " -> " << newName
                     << " (Qty: " << newQty << ")" << endl;
                found = true;
                break;
            }
            cur = cur->next;
        }
        if (!found) {
            cout << "Item not found: " << oldName << endl;
        }
    }

};

// Function to display menu for Warehouse System
void displayWarehouseMenu() {
    cout << "\n=== Warehouse Inventory and Shipping System ===" << endl;
    cout << "1. Add Incoming Item" << endl;
    cout << "2. Process Incoming Item" << endl;
    cout << "3. Ship Item" << endl;
    cout << "4. View Last Incoming Item" << endl;
    cout << "5. View Next Shipment" << endl;
    cout << "6. View All Items" << endl;
    cout << "7. Remove Item by Name\n";
    cout << "8. Search Item\n";
    cout << "9. Count Items\n";
    cout << "10. Update Item\n"; // <-- Added menu option
    cout << "11. Exit\n";
    cout << "Enter your choice: ";
}

// Function to run the Warehouse System
void runWarehouseSystem() {
    WarehouseSystem warehouse;
    warehouse.loadFromFile("result.txt");
    int choice;

    do {
        displayWarehouseMenu();
        cin >> choice;

        // Input validation
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number." << endl;
            continue;
        }

        switch (choice) {
            case 1: {
                string itemName;
                int qty;
                cout << "Enter item name: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, itemName);
                cout << "Enter quantity: ";
                cin >> qty;
                if (cin.fail() || qty <= 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid quantity! Must be a positive integer." << endl;
                    break;
                }
                try {
                    bool onlySpaces = true;
                    for (char c : itemName) {
                        if (!isspace(c)) {
                            onlySpaces = false;
                            break;
                        }
                    }
                    if (itemName.empty() || onlySpaces) {
                        throw invalid_argument("Invalid item name! Cannot be empty or only spaces.");
                    }
                    for (char c : itemName) {
                        if (!isalpha(c) && !isspace(c)) {
                            throw invalid_argument("Invalid item name! Only letters are allowed.");
                        }
                    }
                    warehouse.addItem(itemName, qty);
                } catch (const invalid_argument& e) {
                    cout << e.what() << endl;
                }
                break;
            }
            case 2: {
                string itemName;
                int qty;
                cout << "Enter item name to process: ";
                cin.ignore();
                getline(cin, itemName);
                cout << "Enter quantity to process: ";
                cin >> qty;
                if (cin.fail() || qty <= 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid quantity! Must be a positive integer." << endl;
                    break;
                }
                warehouse.processItem(itemName, qty);
                break;
            }
            case 3: {
                string itemName;
                int qty;
                cout << "Enter item name to ship: ";
                cin.ignore();
                getline(cin, itemName);
                cout << "Enter quantity to ship: ";
                cin >> qty;
                if (cin.fail() || qty <= 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid quantity! Must be a positive integer." << endl;
                    break;
                }
                warehouse.shipItem(itemName, qty);
                break;
            }
            case 4:
                warehouse.viewLastIncoming();
                break;
            case 5:
                warehouse.viewNextShipment();
                break;
            case 6:
                warehouse.viewAll();
                break;
            case 7: {
                string name;
                cout << "Enter item name to remove: ";
                cin.ignore();
                getline(cin, name);
                warehouse.removeItem(name);
                break;
            }
            case 8: {
                string searchName;
                cout << "Enter item name to search: ";
                cin.ignore();
                getline(cin, searchName);
                warehouse.searchItem(searchName);
                break;
            }
            case 9: {
                warehouse.countItems();
                break;
            }
            case 10: {
                string oldName, newName;
                int newQty;
                cout << "Enter current item name to update: ";
                cin.ignore();
                getline(cin, oldName);
                cout << "Enter new item name: ";
                getline(cin, newName);
                cout << "Enter new quantity: ";
                cin >> newQty;
                if (cin.fail() || newQty <= 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid quantity! Must be a positive integer." << endl;
                    break;
                }
                warehouse.updateItem(oldName, newName, newQty);
                break;
            }
            case 11:
                cout << "Exiting Warehouse System. Goodbye!" << endl;
                warehouse.saveToFile("result.txt");
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 11);
}

int main() {
    runWarehouseSystem();

    return 0;
}
