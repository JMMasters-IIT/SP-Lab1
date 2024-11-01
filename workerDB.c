#include <stdio.h>
#include <string.h>
#include "readfile.h"

#define MAXEMPLOYEES 1024
#define MAXNAME 64

//Jack Miller 10.31.2024
//This program is designed to input a file from the command line when it's run
//and build a database with the data from the input file. It will skip any entry that has a space
//in the first or last name field, and skip any line that has a salary <30000 and >150000.
//It can read out and count the number of entries it imported. Search for someone based on ID.
//Search for someone based on the last name. And, finally, it will add an entry into the db
//by prompting the user for the first name, last name, and salary and perform the same data validation
//techniques mentioned above. It will then auto assign an ID based on the highest ID number
//in the db and increment by 1 from there for each additional entry.

typedef struct {
    int id;
    char first_name[MAXNAME];
    char last_name[MAXNAME];
    float salary;
} Employee;

Employee employees[MAXEMPLOYEES];
int employee_count = 0;

//function prototypes
void print_db();
void lookup_by_id();
void lookup_by_last();
void add_emp();
int nextID();
void clear_input();


void clear_input() { //used to prevent add_emp overflow from first name to last name if there's a space
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void load_data(const char *filename) {
    if (open_file(filename) == -1) {
        printf("Error: Could not open file %s.\n", filename);
        return;
    }
    Employee temp;
    while (read_int(&temp.id) == 0 &&
           read_string(temp.first_name, MAXNAME) == 0 &&
           read_string(temp.last_name, MAXNAME) == 0 &&
           read_float(&temp.salary) == 0) {

        if (temp.id < 100000 || temp.id > 999999 || temp.salary < 30000 || temp.salary > 150000) {
            printf("Skipping invalid entry: ID=%d, Name=%s %s, Salary=%.0f\n", temp.id, temp.first_name, temp.last_name, temp.salary);
            continue;
        }
        employees[employee_count++] = temp;
        if (employee_count >= MAXEMPLOYEES) {
            printf("Database full; cannot load more employees.\n");
            break;
        }
    }
    close_file();
}

void print_db() { //prints the database and count of people
    printf("\n%-15s %-15s %-10s %-10s\n", "First Name", "Last Name", "Salary", "ID");
    printf("---------------------------------------------------\n");
    for (int i = 0; i < employee_count; i++) {
        printf("%-15s %-15s $%-9.0f %-10d\n", 
               employees[i].first_name, employees[i].last_name,
               employees[i].salary, employees[i].id);
    }
    printf("---------------------------------------------------\n");
    printf("Total number of employees: %d\n\n", employee_count); //print number of people in db
}

void lookup_by_id() { //function to lookup a person by their ID
    int id;
    printf("Enter ID to lookup: ");
    scanf("%d", &id);
    //For loop for linear search through db
    for (int i = 0; i < employee_count; i++) {
        if (employees[i].id == id) { //Compare input to each value in db to find a match
            printf("Employee Found: ID: %06d, Name: %s %s, Salary: $%.2f\n", 
                   employees[i].id, employees[i].first_name, 
                   employees[i].last_name, employees[i].salary);
            return;
        }
    }
    printf("Employee with ID %06d not found.\n", id); 
}

void lookup_by_last() { //Function to lookup by last name, it is case sensitive
    char last_name[MAXNAME];
    printf("Enter the employee's last name: ");
    scanf("%s", last_name);
    //For loop for linear search through db
    for (int i = 0; i < employee_count; i++) {
        if (strcmp(employees[i].last_name, last_name) == 0) { //Compare input to each name in db to find a match
            printf("Employee Found: ID: %06d, Name: %s %s, Salary: $%.2f\n", 
                   employees[i].id, employees[i].first_name, 
                   employees[i].last_name, employees[i].salary);
            return;
        }
    }
    printf("Employee with last name %s not found.\n", last_name);
}

int nextID() { //Used in add_emp(). Checks for the highest ID currently in the db and increments by 1 for new ID
    int max_id = 100000;  //start at lowest possible ID
    for (int i = 0; i < employee_count; i++) {
        if (employees[i].id > max_id) {
            max_id = employees[i].id;
        }
    }
    //If 999999 is reached, then return -1
    if (max_id >= 999999) {
        return -1;
    }
    return max_id + 1;  //Return highest ID+1 for new ID
}

void add_employee() { //Function to add an employee, validates inputs, asks for confirmation
    if (employee_count >= MAXEMPLOYEES) {
        printf("Unfortunately no more employees can be added to the database.\n");
        return;
    }
    Employee new_employee;
    int next_ID = nextID();
    if (next_ID == -1) { //If we are already at the highest ID then we can't add any new employees
        printf("Cannot add more employees. Maximum employee ID (999999) reached.\n");
        return;
    }
    new_employee.id = next_ID;

    do {
        printf("Enter the first name of the employee: ");
        scanf("%63s", new_employee.first_name);
        clear_input(); //Clear any extra input left
        //This will ignore any spaces in the name and only capture what comes before the space
    } while (strchr(new_employee.first_name, ' ') != NULL);

    do {
        printf("Enter the last name of the employee: ");
        scanf("%63s", new_employee.last_name);
        clear_input(); // Clear any extra input left
        //This will ignore any spaces in the name and only capture what comes before the space
    } while (strchr(new_employee.last_name, ' ') != NULL);

    do {
        printf("Enter employee's salary (30000 to 150000): ");
        scanf("%f", &new_employee.salary);
        if (new_employee.salary != (int)new_employee.salary || //Check that salary is a whole number and between 30000 and 150000
            new_employee.salary < 30000 || new_employee.salary > 150000) {
            printf("Please enter a number between 30,000 and 150,000.\n"); 
        }
    } while (new_employee.salary != (int)new_employee.salary || 
             new_employee.salary < 30000 || new_employee.salary > 150000);

    int confirm;
    //Print out employee to ask if they're sure they want to add them
    printf("\nDo you want to add the following employee to the DB?\n");
    printf("%s %s, Salary: $%.0f\n", new_employee.first_name, new_employee.last_name, new_employee.salary);
    printf("Enter 1 for yes, 0 for no: ");
    scanf("%d", &confirm);

    //Depending on choice it will either add employee to the end of the db or print out they weren't added
    if (confirm == 1) {
        employees[employee_count++] = new_employee;
    } else {
        printf("Employee was not added to the db\n");
    }
}

int main(int argc, char *argv[]) { //Main function for displaying options
    if (argc != 2) {
        printf("Usage: %s <employee_data_file>\n", argv[0]);
        return 1;
    }
    load_data(argv[1]); //Load data from file passed as command line argument

    int choice;
    do {
        printf("\nEmployee Database Menu\n"); //Print menu of options
        printf("1. Print the Database\n");
        printf("2. Lookup employee by ID\n");
        printf("3. Lookup employee by last name\n");
        printf("4. Add an Employee\n");
        printf("5. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            print_db();
        }
        else if (choice == 2){
            lookup_by_id();
        }
        else if (choice == 3){
            lookup_by_last();
        }
        else if (choice == 4){
            add_employee();
        }
        else if (choice == 5){
            printf("Goodbye!\n");
            break;
        }
        else {
            printf("That's not right. Please try again...\n"); 
        }
        } 
    while (choice != 5);

    return 0;
}
