
#include "Index.hpp"

using namespace std;

// Node class implementation

Node::Node(int i){
  item = i;
  prev = NULL;
  next = NULL;
}

void Node::lock(){
  mutex.lock();
}

void Node::unlock(){
  mutex.unlock();
}

Node* Node::getPrev(){
  return prev;
}

Node* Node::getNext(){
  return next;
}

int Node::getItem(){
  return item;
}

void Node::setPrev(Node* p){
  prev = p;
}

void Node::setNext(Node* n){
  next = n;
}

// Index class implementation

Index::Index(int seeds[], int length){
  head = NULL;
  tail = NULL;

  // insert the seed elements into the index
  for(int i=0; i<length; i++){
    bool ret = insert(seeds[i]);
  }
}

void Index::lock(){
  mutex.lock();
}

void Index::unlock(){
  mutex.unlock();
}


bool Index::search(int key){
  int curKey;
  this->lock();
  Node *curNode = head;
  Node *prevNode;
  if(curNode == NULL){
    this->unlock();
    return false;
  } else {
    curNode->lock();
    this->unlock();
  }
  while(curNode != NULL){
    curKey = curNode->getItem();
    if(key == curKey){
      curNode->unlock();
      return true; // key found!
    } else if(key > curKey){
      curNode->unlock();
      return false; // key not found
    }
    prevNode = curNode; // make prevNode point to the same object as curNode
    curNode = curNode->getNext(); // change curNode's pointer to point to the next object. Does not change prevNode's pointer
    if(curNode != NULL){ // curNode is a local variable, so will not change locally
      curNode->lock(); // thus, when lock() is called, curNode will not be NULL locally!
    }
    prevNode->unlock();
  }
  return false; // key not found
}

void insertNode(int key, Node *curNode, Node *nextNode) {
  Node *newNode = new Node(key);
  newNode->setPrev(curNode);
  newNode->setNext(nextNode);
  if(curNode != NULL){
    curNode->setNext(newNode);
  }
  if(nextNode != NULL){
    nextNode->setPrev(newNode);
  }
}

bool Index::insert(int key){
  Node *prevNode;
  this->lock();
  Node *curNode = head;
  Node *nextNode;
  if(curNode == NULL){
    insertNode(key, head, tail);
    this->unlock();
    return true;
  } else {
    curNode->lock();
    nextNode = curNode->getNext();
  }

  if(nextNode != NULL){
    nextNode->lock();
  }
  this->unlock();
  int curKey;
  while(curNode != NULL){
    curKey = curNode->getItem();
    if(key == curKey){
      curNode->unlock();
      if(nextNode != NULL){
        nextNode->unlock();
      }
      return false; // key already in index
    } else if(key > curKey || nextNode == NULL){
      insertNode(key, curNode, nextNode);
      curNode->unlock();
      if(nextNode != NULL){
        nextNode->unlock();
      }
      return true; // inserted key
    }
    prevNode = curNode;
    curNode = nextNode;
    if(nextNode != NULL){
      nextNode = nextNode->getNext();
    }
    if(nextNode != NULL){
      nextNode->lock();
    }
    prevNode->unlock();
  }
  insertNode(key, prevNode, curNode);
  return true;
}

bool Index::remove(int key){
  Node *curNode = head;
  int curKey;
  while(curNode != NULL){
    curKey = curNode->getItem();
    if(key == curKey){
      Node *prevNode = curNode->getPrev();
      Node * nextNode = curNode->getNext();
      if(prevNode != NULL){
        prevNode->setNext(nextNode);
      }
      if(nextNode != NULL){
        nextNode->setPrev(prevNode);
      }
      delete curNode;
      return true;
    } else if(key > curKey){
      return false; // could not find key
    }
    curNode =curNode->getNext();
  }
  return false;
}

void Index::printIndex(char order){

  Node* start;

  // configure traversal order
  if(order == '<'){
    start = head;
  } else if(order == '>'){
    start = tail;
  } else {
    cerr << "ERROR: unrecognised input order " << order << endl;
    return;
  }

  Node* cur = NULL;
  Node* nxt = start;
  cout << "|--";

  // traverse the index elements
  while(nxt != NULL){

    if(nxt != start){
      cout<< "<-->";
    }

    cur = nxt;
    if(order == '<'){
      nxt = cur->getNext();
    } else {
      nxt = cur->getPrev();
    }

    cout << "|·|" << cur->getItem() << "|·|";
  }

  cout << "--|" << endl;
}
