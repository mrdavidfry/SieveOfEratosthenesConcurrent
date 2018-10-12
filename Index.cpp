
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
std::mutex mtx;
bool Index::search(int key){
  mtx.lock();
  int curKey;
  Node *curNode = head;
  Node *prevNode = NULL;
  while (curNode != NULL){
    curKey = curNode->getItem();
    if(key == curKey){
      mtx.unlock();
      return true; // key found!
    } else if(key > curKey){
      mtx.unlock();
      return false; // key not found
    }
    prevNode = curNode;
    curNode = curNode->getNext();
    prevNode->unlock();
  }
  mtx.unlock();
  return false; // key not found
}

void insertNode(int key, Node *prevNode, Node *nextNode) {
  Node *newNode = new Node(key);
  newNode->setPrev(prevNode);
  newNode->setNext(nextNode);
  if (prevNode != NULL){
    prevNode->setNext(newNode);
  }
  if (nextNode != NULL){
    nextNode->setPrev(newNode);
  }
}

bool Index::insert(int key){
  mtx.lock();
  Node *prevNode = NULL;
  Node *curNode = head;
  if (curNode == NULL) {
    insertNode(key, prevNode, NULL);
    return true;
  }
  Node *nextNode = curNode->getNext();
  int curKey;
  while (curNode != NULL){
    curKey = curNode->getItem();
    if (key == curKey){
      mtx.unlock();
      return false; // key already in index
    } else if(key > curKey || nextNode == NULL) {
      insertNode(key, prevNode, nextNode);
      mtx.unlock();
      return true; // inserted key
    }
    prevNode = curNode;
    curNode = nextNode;
    nextNode = nextNode->getNext();
  }
  insertNode(key, prevNode, nextNode);
  mtx.unlock();
  return true;
}


bool Index::remove(int key){
  mtx.lock();
  Node *curNode = head;
  int curKey;
  while (curNode != NULL){
    curKey = curNode->getItem();
    if (key == curKey){
      Node *prevNode = curNode->getPrev();
      Node * nextNode = curNode->getNext();
      if (prevNode != NULL){
        prevNode->setNext(nextNode);
      }
      if (nextNode != NULL){
        nextNode->setPrev(prevNode);
      }
      delete curNode;
      mtx.unlock();
      return true;
    } else if (key > curKey) {
      mtx.unlock();
      return false; // could not find key
    }
    curNode =curNode->getNext();
  }
  mtx.unlock();
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
