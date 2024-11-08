#include "TrinomialPrice.h"
#include <math.h>

// Set up the Trinomial Pricing Parameters
TrinomialPrice::TrinomialPrice(Option* pOpt, int nSteps)
        : option(pOpt), steps(nSteps)

// TO DO:
{
    dt = (pOpt->maturity)/nSteps;
    u = exp(lambda*(pOpt->vol)* sqrt(dt));
    d = 1/u;
    double M = exp((pOpt->rate)*dt);
    double V = M*M * (exp(pow((pOpt->vol),2)*dt)-1);
    pu = ((V+M*M-M)*u-(M-1))/((u-1)*(u*u-1));
    pd = ((V+M*M-M)*u*u-(M-1)*pow(u,3))/((u-1)*(u*u-1));
    ph = 1-pu-pd;
    discount = exp(-(pOpt->rate)*dt);
    lattice = new Lattice();
    lattice->setLevel(steps);
};

// Build the Trinomial Lattice recursively
void
TrinomialPrice::buildLattice(Node* t)
// TO DO:

{
    if(t==nullptr){
        t = new Node(option->spotPrice);
        t->level=0;
        lattice->root=t;
    }
    Node* unode = new Node(t->data.sharePrice*u);
    Node* hnode = new Node(t->data.sharePrice);
    Node* dnode = new Node(t->data.sharePrice*d);
    unode->sibling = hnode;
    hnode->sibling = dnode;
    dnode->sibling = unode;
    unode->level = t->level+1;
    hnode->level = t->level+1;
    dnode->level = t->level+1;
    t->up = unode;
    t->horizontal = hnode;
    t->down = dnode;
    if(t->level>=lattice->getLevel())return;
    buildLattice(t->up);
    buildLattice(t->horizontal);
    buildLattice(t->down);
}

// Calculate the option value using recursion
void
TrinomialPrice::backwardEval(Node* t)
// TO DO:
{
    if(t->up == nullptr){
        t->data.optionPrice = MAX(OPTION_MULTIPLIER(option->c_p)*(t->data.sharePrice-option->strikePrice),0);
        return;
    }
    backwardEval(t->up);
    backwardEval(t->horizontal);
    backwardEval(t->down);
    if(t->up != nullptr) {
        double o_value_up = t->up->data.optionPrice;
        double o_value_h = t->horizontal->data.optionPrice;
        double o_value_down = t->down->data.optionPrice;
        if(option->e_a==European){
            t->data.optionPrice = discount * (pu * o_value_up + ph * o_value_h + pd * o_value_down);
        }
        else{
            double early_exe = MAX(OPTION_MULTIPLIER(option->c_p)*(t->data.sharePrice-option->strikePrice),0);
            double not_early_exe = discount * (pu * o_value_up + ph * o_value_h + pd * o_value_down);
            t->data.optionPrice = MAX(early_exe,not_early_exe);
        }
        return;
    }
}