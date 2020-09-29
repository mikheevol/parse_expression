/*
 * node.h
 *
 *  Created on: 15.09.2020
 *      Author: Mikheev Oleg
 */

#ifndef NODE_H_
#define NODE_H_

#include <iostream>

typedef std::vector<Token>::iterator _iter;

struct Node {
	Node(Token _data) :
		parent(0), left(0), right(0), item(_data) {
	}
	~Node() {
	}
	Node *parent;
	Node *left;
	Node *right;

	Token item;
};

// erease tree or subtree
void EraseTree(Node *root) {
	if (root) {
		EraseTree(root->left);
		EraseTree(root->right);
		delete root;
		root = 0;
	}
}

// insert node into tree of tokens according with lexem priority
Node* InsertNode(Node* ptr, Token _data) {
	Node *t = new Node(_data);

	if (ptr) {
		if (ptr->item.data.pr >= t->item.data.pr) {
			if (ptr->right) {
				// we allready have right child
				if (t->item.data.pr == PR_NUMERIC_OR_EXPRESION) {
					std::cout << "syntax error" << std::endl;
					delete t;
					t = 0;
				} else {
					t->left = ptr->right;
					t->parent = ptr;
					ptr->right = t;
				}
			} else {
				ptr->right = t;
				t->parent = ptr;
			}
		} else if (ptr->item.data.pr < t->item.data.pr) {
			Node * par = ptr->parent;

			if (par == 0) {
				t->parent = par;
				t->left = ptr;
				ptr->parent = t;
			} else {
				if (par->item.data.pr > t->item.data.pr) {
					t->parent = par;
					t->left = ptr;
					ptr->parent = t;
					par->right = t;
				} else {
					if (par->parent != 0) {
						t->parent = par->parent;
					} else {
						t->parent = 0;
					}
					t->left = par;
					par->parent = t;
				}
			}
		}
	} else {
		t->parent = 0;
	}
	return t;
}

// Create tree from vector of tokens
// params:
//	itfirst - first element of token vector
//	itlast - last element of token vector
//	subexpr - a sign of being inside subexpression
//	pos - a pointer to iterator of token vector in case exit from subexpression
//	root - returning pointer to tree root element
Node* CreateTree(_iter itfirst, _iter itlast, _iter *pos, bool subexpr) {
	Node *root = 0;
	Node *ptr = 0;
	int error = 0;

	bool SubDone = false;

	for (_iter itv = itfirst; itv < itlast; ++itv) {
		if (itv->data.pr == PR_PARENTES) {
			if (itv->val == "(") {
				// subexpression begins
				_iter i;
				Node *sub = CreateTree(itv + 1, itlast, &i, true);

				if (sub == 0) {
					error = 1;
					break;
				} else {
					if (ptr) {
						if (ptr->item.data.pr < PR_DIVISON_PRODUCT) {
							EraseTree(sub);
							sub = 0;
							error = 1;
							std::cout << "syntax error: missing operator" << std::endl;
							break;
						} else {
							ptr->right = sub;
							sub->parent = ptr;
							sub->item.data.pr = PR_NUMERIC_OR_EXPRESION;
						}
					} else {
						ptr = sub;
						sub->item.data.pr = PR_NUMERIC_OR_EXPRESION;
					}
				}
				// update iterator for parent function
				if (i < itlast) {
					itv = i;
				}
			} else if (itv->val == ")") {
				if (subexpr) {
					// close subexpression
					*pos = itv;
					if (ptr) {
						if (ptr->parent == 0) {
							root = ptr;
						}
					} else {
						error = 1;
						std::cout << "syntax error: empty expression" << std::endl;
					}
					SubDone = true;
					break;
				} else {
					// close parentes can't be first
					std::cout << "syntax error: close parentes ')' in absence of open parentes '('"	<< std::endl;
					error = 1;
					break;
				}
			}
		} else {
			if (ptr == 0) {
				ptr = InsertNode(root, *itv);
			} else {
				ptr = InsertNode(ptr, *itv);
				if (ptr == 0) {
					// if empty expression
					std::cout << "syntax error: missing operator" << std::endl;
					error = 1;
				}
			}
			if (ptr->parent == 0) {
				root = ptr;
			}
		}
		if (ptr->parent == 0) {
			root = ptr;
		}
	}
	if (subexpr) {
		if (!SubDone) {
			error = 1;
			std::cout << "syntax error: missing close parentes ')'"	<< std::endl;
		}
	}
	// processing errors
	if (error) {
		// in case of errors destroy parse tree
		if (root) {
			EraseTree(root);
			root = 0;
		}
	}
	return root;
}
#endif /* NODE_H_ */
