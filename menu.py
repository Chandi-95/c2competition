import sys
import os
from server import *

# Main definition - constants
menu_actions = {}  


# MENU FUNCTIONS

# Main menu
def main_menu():
    os.system('cls')
    
    print("Welcome,")
    print("Please choose the menu you want to start:")
    print("1. Connections")
    #print("2. "")
    print("0. Exit")
    choice = input(">>  ")
    exec_menu(choice)
    return

# Execute menu
def exec_menu(choice):
    os.system('cls')
    ch = choice.lower()
    if ch == '':
        menu_actions['main_menu']()
    else:
        try:
            menu_actions[ch]()
        except KeyError:
            print("Invalid selection, please try again.")
            menu_actions['main_menu']()
    return

# Menu 1
def menu1():
    while True: 
        print("Choose an option below: ")
        print("3. List active connections")
        print("4. Create a connection")
        print("5. Rename a connection")
        print("6. Interact with a connection")
        print("9. Back")
        print("0. Exit")
        choice = input(">>  ")
        exec_menu(choice)
        #return

# Menu 2
# def menu2():
#     print("Hello Menu 2 !")
#     print("9. Back")
#     print ("0. Quit") 
#     choice = input(">>  ")
#     exec_menu(choice)
#     return

# Back to main menu
def back():
    menu_actions['main_menu']()

# Exit program
def exit():
    sys.exit()

# MENU ACTION DEFINITIONS

# Menu definition
menu_actions = {
    'main_menu': main_menu,
    '1': menu1,
    #'2': menu2,
    '3': listconnections,
    '4': createconnections,
    '5': renameconnections,
    '6': interactconnections,
    '9': back,
    '0': exit,
}

# Main Program
# if __name__ == "__main__":
#     Launch main menu
#     main_menu()