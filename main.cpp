#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Item{
private:
    string name;
public:
    Item(const string &n) : name(n) {}

    // methods using upcasting (converting a derived class obj to a base class obj)
    virtual const string &getName() const // the private data member can not be modified since it's a const reference
    {
        return name;
    }
    virtual const int &getSize() const
    {
        return 0;
    }
    /* VAR1:
    virtual ostream& display(ostream& out) const // in this case, the classes file and directory must display the assigned name, then a file must include other details
    {
        return out<<name;
    } */
    friend ostream &operator<<(ostream &out, const Item &obj)
    {
        out<<obj.getName(); // name is a private member, therefore i must use getName()
        return out;
    }
};

class File : public Item{
private:
    int size;
    string extension;
public:
    File(const string &n, const string &ext, int s) : Item(n), extension(ext), size(s) {}
    File(const string &n, const string &ext) : Item(n), extension(ext) {}
    const int &getSize() const
    {
        return size;
    }
    const string &getExtension() const
    {
        return extension;
    }
    /* VAR1:
    ostream& display(ostream& out) const
    {
        return Item::display(out)<<"."<<extension<<" "<<size; // using scope resolution operator since we must access the method display() defined in the base class item
    }*/
    friend ostream &operator<<(ostream &out, const File &obj)
    {
        out<<static_cast<const Item &>(obj); // upcasting since we must convert a derived class pointer to the base class pointer (any file has a name, which is an item's attribute)
        out<<"."<<obj.getExtension()<<" "<<obj.getSize();
        return out;
    }
};

class Directory : public Item{
private:
    vector<Item *> path;
public:
    Directory(const string &n) : Item(n) { path.clear(); } // there's no need to declare the vector variable here
    ~Directory()
    {
        if(!this->path.empty())
        {
            this->path.clear();
        }
    }
    void addItem(Item *obj) // pointing at an item
    {
        path.push_back(obj); // adding objects to the current directory path
    }
    Directory *navigateTo(const string& name) // function which returns a pointer to a directory
    {
        // for every item located within the path, declared as references
        // might have to implement exception handling block
        for (const auto& item : path)
        {
            // if the name corresponds and it is a directory
            if (item->getName() == name && dynamic_cast<Directory*>(item)) // we must use dynamic_cast since the item should be checked whether it is a directory or not
            {
                return dynamic_cast<Directory*>(item);
            }
        }
        return nullptr;
    }
    void addDirectory(const string& name)
    {
        Directory *new_dir = new Directory(name);
        addItem(new_dir); // addItem() is part of the directory class methods (don't be mistaken), otherwise i should've used the scope resolution operator
    }
    int getNumberOfItems() // the number of direct children nodes is the size on the path vector for the current directory
    {
        return path.size(); // size() instead of length() since we use elements in the vector path of type pointer
    }
    int getSize()
    {
        int total_size = 0;
        for(const auto &item : path)
        {
            Directory *subdir = dynamic_cast<Directory*>(item);
            if(subdir)
            {
                total_size += subdir->getSize(); // sizes of files located in each subdir
            } else
            {
                total_size += item->getSize();
            }
        }
        return total_size;
    }
    void syntaxHelper(std::ostream& out, int k) const
    {
        for (int i = 0; i < k; i++)
        {
            out<<" "; // increasing the indentation in relation to directory-subdirectory depth
        }
        if (k == 1)
        {
            out<<"|-"; // hyphen = the immediate nodes located within the parent folder
        } else if (k != 0)
        {
            out<<"|_"; // underscore = every other level of items contained in the parent's folder direct children
        }
    }
    friend ostream &operator<<(ostream &out, const Directory &obj) // operator<< overloading
    {
        // the indentation is specific to every level, meaning it should be consistent for items within the same subdir
        static int k = 0; // static data since there are recursive calls within this function, meaning k must retain its value
        obj.syntaxHelper(out, k);
        out<<obj.getName()<<endl;
        k += 1; // descending into a subdir
        for(const auto &item : obj.path)
        {
            Directory *subdir = dynamic_cast<Directory*>(item);
            if(subdir) // if it exists, it lists all of its items recursively
            {
                out<<*subdir; // !!! dereferencing the pointer
            } else
            {
                obj.syntaxHelper(out, k);
                // VAR1: item->display(out);
                out<<*dynamic_cast<File *>(item); // !!! dereferencing + downcasting since we must convert a base class pointer to a derived class pointer
                out<<endl;
            }
        }
        k -= 1; // returning to the parent directory, since it might have multiple children nodes, meaning the indentation will reset by exactly one position
        return out;
    }
};

int main() {
    // item i; can't make an object of class i
    Directory root("/");
    root.addItem(new Directory("bin"));
    root.addItem(new Directory("home"));
    root.addItem(new File("image1", "png", 20));
    Directory *home = root.navigateTo("home");
    home->addDirectory("tim");
    Directory *tim = home->navigateTo("tim");
    tim->addItem(new File("info", "txt", 100));
    tim->addItem(new File("data", "in"));
    tim->addItem(new Directory("data"));
    Directory *bin = root.navigateTo("bin");
    bin->addItem(new File("program", "exe", 450));

    cout<<root.getSize()<<endl;
    cout<<root.getNumberOfItems()<<endl;
    cout<<root;
    // item hierarchy
    /*
        570
        3
        /
         |-bin
          |_program.exe 450
         |-home
          |_tim
           |_info.txt 100
           |_data.in 0
           |_data
         |-image1.png 20
    */
}