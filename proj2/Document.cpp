#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Document.h"

using namespace std;

// DOCUMENT METHODS-----------------------------------------------------------------

/*
  Constructor:
    The constructor function is responsible for constructing a new node in the linked list
*/
Line::Line(string new_line, Line *new_next_node){
  line = new_line;
  next_node = new_next_node;
}

// Document METHODS-------------------------------------------------------------

/*
  ~Document() -- destructor
    This method loops through the entire linked list and frees all node from the heap
*/
Document::~Document(){
  Line *curr_ptr = head;
  Line *next_ptr = NULL;

  while (curr_ptr != NULL){ // loop until end of list or line number found
    next_ptr = curr_ptr->next_node; // go to next node
    delete curr_ptr; // free the memory of the current node
    curr_ptr = next_ptr;
  }

  curr_ptr = undo_head;
  while (curr_ptr != NULL){ // loop until end of list or line number found
    next_ptr = curr_ptr->next_node; // go to next node
    delete curr_ptr; // free the memory of the current node
    curr_ptr = next_ptr;
  }

  curr_ptr = redo_head;
  while (curr_ptr != NULL){ // loop until end of list or line number found
    next_ptr = curr_ptr->next_node; // go to next node
    delete curr_ptr; // free the memory of the current node
    curr_ptr = next_ptr;
  }
}

/*
  get_num_lines():
    This method is responsible for finding the total number of lines in the document.
    This can be useful when determining where to insert a line.
*/
int Document::get_num_lines(){

  Line *curr_ptr = head;
  int count = 0;

  while (curr_ptr != NULL){ // count the number of lines until curr_ptr == NULL OR end of linked list
    count += 1; // increment count
    curr_ptr = curr_ptr->next_node; // move to next node
  }

  return count;

}

/*
  insert_line():
    This method inserts a new line into the document.
    Inputs: a line of type string, and the line number to insert at
    If the user chooses to insert the line at the end of the document, then get_num_lines() is called prior to
      invoking this method.
    There are five cases this method handles:
      - The document is empty
      - Inserting at the head of an existing list
      - Inserting in the middle of a list
      - Inserting at the end of a list
      - Inserting at a line beyond the current eof
*/
void Document::insert_line(string line, int line_number){

  copy_linked_list(head, undo_head);

  Line *curr_ptr = head;
  Line *prev_ptr = NULL;
  Line *new_line_ptr;

  new_line_ptr = new Line(line, curr_ptr); // new node

  if (head == NULL){ // case where list empty
    head = new_line_ptr;
  }
  else {

    int line_count = 1;
    Line *pad_list_with_blank_line;
    bool is_padding_more_than_one = false;
    while (line_count <= line_number){ // loop until end of list or line number found
      if (curr_ptr == NULL){ // case where we are now inserting past the end of the current end of file
        Line *temp;
        if (!is_padding_more_than_one){ // case where we pad just one line
          pad_list_with_blank_line = new Line("", NULL);
          prev_ptr->next_node = pad_list_with_blank_line;
          temp = pad_list_with_blank_line;
          is_padding_more_than_one = true;
        }
        else { // case where we pad more than one line -- a temporary pointer must be used
          pad_list_with_blank_line = new Line("", NULL);
          prev_ptr = temp;
          prev_ptr->next_node = pad_list_with_blank_line;
          temp = pad_list_with_blank_line;
        }
        delete pad_list_with_blank_line;
      }
      else {
        prev_ptr = curr_ptr;
        curr_ptr = curr_ptr->next_node; // go to next node
      }
      line_count += 1; // increment line counter
    }

    if (curr_ptr == NULL){ // case where insertion is at the end of the list
      prev_ptr->next_node = new_line_ptr;
      new_line_ptr->next_node = NULL; // update new node ptr to NULL (now at tail)
    }
    else if (prev_ptr == NULL){ // case where insertion is at the beginning of an existing list
      head = new_line_ptr; // head now points to new node
      new_line_ptr->next_node = curr_ptr;
    }
    else { // case where insertion is in the middle of the list
      prev_ptr->next_node = new_line_ptr;
      new_line_ptr->next_node = curr_ptr;
    }

  }

}

/*
  delete_line():
    This method is responsible for deleting nodes in the linked list
    Inputs: line number to delete
    Method must check if the line number exists in the document
    method must call delete keyword after repointing the correct nodes
*/
void Document::delete_line(int line_number){

  if (line_number > get_num_lines() || line_number < 1){ // case where the line number does not exist in the document
    cout << "You are trying to delete a line that does not exist\n";
  }
  else{

      copy_linked_list(head, undo_head);

      Line *curr_ptr = head;
      Line *prev_ptr = NULL;

      int line_count = 1;
      while (curr_ptr != NULL && line_count < line_number){ // loop until end of list or line number found
        prev_ptr = curr_ptr;
        curr_ptr = curr_ptr->next_node; // go to next node
        line_count += 1; // increment line counter
      }

      prev_ptr->next_node = curr_ptr->next_node;
      delete curr_ptr; // delete the node -- free up memory

  }

}

/*
  print_document():
  This method is responsible for print all lines in the document
  The method iterates over the entire list and prints the 'line' private data member within each node
*/
void Document::print_document(){

  Line *curr_ptr = head;

  if (curr_ptr == NULL){ // case where the document is empty
    cout << "This is an empty document\n";
  }
  else {

      int line_number = 1;
      while (curr_ptr != NULL){ // loop until end of list
        cout << line_number << "> " << curr_ptr->line << endl; // print
        curr_ptr = curr_ptr->next_node; // move to next line
        line_number += 1; // increment line counter
      }

  }

}

/*
  save_document():
    This method is responsible for saving the newly edited document
    Inputs: the file name
    Method must check if the document is empty
*/
void Document::save_document(string filename){

  Line *curr_ptr = head;

  if (curr_ptr == NULL){
    cout << "This is an empty document\n"; // inform the user that the document is empty
    cout << "Saving an empty document\n";
  }

  ofstream file(filename); // open an output stream

  while (curr_ptr != NULL){ // loop until end of list
    file << curr_ptr->line << endl; // append line to file
    curr_ptr = curr_ptr->next_node; // go to next node
  }

  file.close(); // clode the file stream

}

/*
  copy():
    This method is responsible for copying the text at the specified line number
    Inputs: line number
    Method must check if the document is empty and if the line number is valid
*/
void Document::copy(int copy_line_number){
  Line *curr_ptr = head;

  if (curr_ptr == NULL){ // case where the document is empty
    cout << "This is an empty document\n";
  }
  else {

      // find the line to copy
      int line_number = 1;
      bool line_found = false;
      while (curr_ptr != NULL){ // loop until end of list
        if (line_number == copy_line_number){ // case where the line number was found
          line_found = true;
          break;
        }
        curr_ptr = curr_ptr->next_node; // move to next line
        line_number += 1; // increment line counter
      }

      if (line_found){
        copy_line = curr_ptr->line; // assign the string to the data member copy_line
      }
      else {
        cout << "You tried to copy a non-existent line\n";
      }

  }
}

/*
  paste():
    This method is responsible for pasting (inserting) the text at the specified line number
    Inputs: line number
    Method must check if the document is empty and if the line number is valid
*/
void Document::paste(int paste_line_number){

  Line *curr_ptr = head;
  int number_of_lines = 0;

  if (curr_ptr == NULL){ // case where the document is empty
    cout << "This is an empty document\n";
  }
  else {
      // count the number of lines
      while (curr_ptr != NULL){ // loop until end of list
        curr_ptr = curr_ptr->next_node; // move to next line
        number_of_lines += 1; // increment line counter
      }

  }

  // check if the paste line number is a valid line number
  if (paste_line_number > 0 && paste_line_number <= number_of_lines){
    copy_linked_list(head, undo_head);
    insert_line(copy_line, paste_line_number-1);
  }
  else {
    cout << "You tried to paste to a non-existent line\n";
  }

}

/*
  delete_list()
    This function is to free the memory of a linked list
      This function is used when copying linked list -- the old list must first be freed
*/
void Document::delete_list(Line *ptr){

  if (ptr != NULL){ // check if the pointer is null
      Line *curr_ptr = ptr;
      Line *next_ptr = NULL;

      while (curr_ptr != NULL){ // loop until end of list or line number found
        next_ptr = curr_ptr->next_node; // go to next node
        delete curr_ptr; // free the memory of the current node
        curr_ptr = next_ptr;
      }
  }

}

/*
  copy_linked_list()
    This function is used to copy one linked list into another
      This function is used for the undo and redo functions
*/
void Document::copy_linked_list(Line *&ptr_src, Line *&ptr_dest){

  if (ptr_src != NULL){
    delete_list(ptr_dest); // delete the old list
    Line *curr_ptr = ptr_src;
    ptr_dest = new Line(curr_ptr->line, NULL); // assign the new head of the list
    Line *curr_undo_ptr = ptr_dest;
    while (curr_ptr != NULL){ // iterate over the entire source list
      Line *new_node = new Line(curr_ptr->line, NULL); // assign new node
      curr_undo_ptr->next_node = new_node; // assign new node to list being copied to
      curr_undo_ptr = new_node;
      curr_ptr = curr_ptr->next_node;
    }
  }

}

/*
  undo()
    This function allows the user to undo a delete or insertion
*/
void Document::undo(){
  copy_linked_list(head, redo_head);
  copy_linked_list(undo_head, head);
}

/*
  redo()
    This function allows the user to redo a recent undo
*/
void Document::redo(){
  copy_linked_list(redo_head, head);
}
