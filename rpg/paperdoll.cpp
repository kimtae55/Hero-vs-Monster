#include <iostream>
#include <string>

using namespace std;

#include "main.h" 
#include "item.h"
#include "unit.h"
#include "paperdoll.h" 

Paperdoll::Paperdoll() {
    head = NULL;
    chest = NULL;
    back = NULL;
    hands = NULL;
    legs = NULL;
    feet = NULL;
    
    rightHand = NULL;
    leftHand = NULL;
}

Paperdoll::~Paperdoll() {
    if (head != NULL) {
        delete head;
    }
    if (chest != NULL) {
        delete chest;
    }
    if (back != NULL) {
        delete back;
    }
    if (hands != NULL) {
        delete hands;
    }
    if (legs != NULL) {
        delete legs;
    }
    if (feet != NULL) {
        delete feet;
    }
    if (rightHand != NULL) {
        delete rightHand;
    }
    if (leftHand != NULL) {
        delete leftHand;
    }
}

void Paperdoll::print() {
    cout << "[ 1:Head(" << ((head == NULL)?' ':head->getShape())
         << ") 2:Chest(" << ((chest == NULL)?' ':chest->getShape())
         << ") 3:Back(" << ((back == NULL)?' ':back->getShape())
         << ") 4:Hands(" << ((hands == NULL)?' ':hands->getShape())
         << ") 5:Legs(" << ((legs == NULL)?' ':legs->getShape())
         << ") 6:feet(" << ((feet == NULL)?' ':feet->getShape())
         << ") | 7:L-Hand(" << ((leftHand == NULL)?' ':leftHand->getShape())
         << ") 8:R-Hand(" << ((rightHand == NULL)?' ':rightHand->getShape())
         << ") ]" << endl;
    
}

bool Paperdoll::equip(Item *item, Unit *unit) {
    // if equippable item has been passed.
    if (item->isEquippable()) {
        EquippableItem *equippableItem = (EquippableItem *)item;
        // if armor has been passed
        if (equippableItem->isArmor()) {
            bool equipped = true;
            Armor *armor = (Armor *)equippableItem;
            
            if (armor->getBodyPartID() == BODY_PART_ID_HEAD) {
				if (head != NULL) {
					return false;
				}
                head = armor;
            }
            else if (armor->getBodyPartID() == BODY_PART_ID_CHEST) {
				if (chest != NULL) {
					return false;
				}
                chest = armor;
            }
            else if (armor->getBodyPartID() == BODY_PART_ID_BACK) {
				if (back != NULL) {
					return false;
				}
                back = armor;
            }
            else if (armor->getBodyPartID() == BODY_PART_ID_HANDS) {
				if (hands != NULL) {
					return false;
				}
                hands = armor;
            }
            else if (armor->getBodyPartID() == BODY_PART_ID_LEGS) {
				if (legs != NULL) {
					return false;
				}
                legs = armor;
            }
            else if (armor->getBodyPartID() == BODY_PART_ID_FEET) {
				if (feet != NULL) {
					return false;
				}
                feet = armor;
            }
            else { // error
                equipped = false;
            }
            
            if (equipped) {
                unit->incDef(armor->getDef());
                return true;
            }
        }
        // if weapon has been passed
        else {
            bool equipped = true;
            Weapon *weapon = (Weapon *)equippableItem;
            
            if (weapon->getNumHands() == DEFAULT_WEAPON_ONE_HAND) {
                // when both hands are free
                if (rightHand == NULL) {
                    rightHand = weapon;
                }
                // right hand occupied, not dual wield, and left hand free
                else if (leftHand == NULL && rightHand->getNumHands() == DEFAULT_WEAPON_ONE_HAND) {
                    leftHand = weapon;
                }
                else {
                    equipped = false;
                }
            }
            else if (weapon->getNumHands() == DEFAULT_WEAPON_DUAL_WIELD) {
                // when both hands are free
                if (rightHand == NULL) {
                    rightHand = weapon;
                }
                else {
                    equipped = false;
                }
            }
            else { // error
                equipped = false;
            }

            if (equipped) {
                unit->incAtk(weapon->getAtk());
                return true;
            }
        }
    }
    
    // non-equippable item has been passed.
    return false;
}

Item *Paperdoll::unequip(int bodyPartID, Unit *unit) {
    bool unequipped = true;
    bool isArmor = true;
    
    Item *itemToUnequip = NULL;
    
    //cout << "Paperdoll::unequip(): bodyPartID=" << bodyPartID << endl;

    if (bodyPartID == BODY_PART_ID_HEAD) {
        //cout << "Paperdoll::unequip(): if (bodyPartID == BODY_PART_ID_HEAD)" << endl;
        itemToUnequip = head;
        head = NULL;
    }
    else if (bodyPartID == BODY_PART_ID_CHEST) {
        itemToUnequip = chest;
        chest = NULL;
    }
    else if (bodyPartID == BODY_PART_ID_BACK) {
        itemToUnequip = back;
        back = NULL;
    }
    else if (bodyPartID == BODY_PART_ID_HANDS) {
        itemToUnequip = hands;
        hands = NULL;
    }
    else if (bodyPartID == BODY_PART_ID_LEGS) {
        itemToUnequip = legs;
        legs = NULL;
    }
    else if (bodyPartID == BODY_PART_ID_FEET) {
        itemToUnequip = feet;
        feet = NULL;
    }
    else if (bodyPartID == BODY_PART_ID_LEFT_HAND) {
        itemToUnequip = leftHand;
        leftHand = NULL;
        isArmor = false;
    }
    else if (bodyPartID == BODY_PART_ID_RIGHT_HAND) {
        //cout << "Paperdoll::unequip(): if (bodyPartID == BODY_PART_ID_RIGHT_HAND)" << endl;
        itemToUnequip = rightHand;
        rightHand = leftHand;
        leftHand = NULL;
        isArmor = false;
    }
    else {
        unequipped = false;
    }
    
    if (unequipped) {
        if (isArmor) {
            Armor *armorUnequipped = (Armor *)itemToUnequip;
            unit->decDef(armorUnequipped->getDef());
        }
        else { // when itemToUnequip is a weapon
            Weapon *weaponUnequipped = (Weapon *)itemToUnequip;
            unit->decAtk(weaponUnequipped->getAtk());
        }
    }

    return itemToUnequip;
}

void Paperdoll::save(ostream &out) {
    out << "#---------------------------------------- class Paperdoll" << endl; 

    out << "#================================================== head" << endl;
    if (head == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        head->save(out);
    }

    out << "#================================================== chest" << endl;
    if (chest == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        chest->save(out);
    }

    out << "#================================================== back" << endl;
    if (back == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        back->save(out);
    }

    out << "#================================================== hands" << endl;
    if (hands == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        hands->save(out);
    }

    out << "#================================================== legs" << endl;
    if (legs == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        legs->save(out);
    }

    out << "#================================================== feet" << endl;
    if (feet == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        feet->save(out);
    }

    out << "#================================================== rightHand" << endl;
    if (rightHand == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        rightHand->save(out);
    }

    out << "#================================================== leftHand" << endl;
    if (leftHand == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        leftHand->save(out);
    }
}

void Paperdoll::load(istream &in) {
    char buf[MAX_LEN_BUF];

    // delete all armors and weapons
    if (head != NULL) {
        delete head;
    }
    if (chest != NULL) {
        delete chest;
    }
    if (back != NULL) {
        delete back;
    }
    if (hands != NULL) {
        delete hands;
    }
    if (legs != NULL) {
        delete legs;
    }
    if (feet != NULL) {
        delete feet;
    }
    if (rightHand != NULL) {
        delete rightHand;
    }
    if (leftHand != NULL) {
        delete leftHand;
    }
    
    //out << "#---------------------------------------- class Paperdoll" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "#================================================== head" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    string tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        head = new Armor();
        
        //head->save(out);
        head->load(in);
    }
    else {
        head = NULL;
    }

    //out << "#================================================== chest" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        chest = new Armor();
        
        //chest->save(out);
        chest->load(in);
    }
    else {
        chest = NULL;
    }
    
    //out << "#================================================== back" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        back = new Armor();
        
        //back->save(out);
        back->load(in);
    }
    else {
        back = NULL;
    }
    
    //out << "#================================================== hands" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        hands = new Armor();
        
        //hands->save(out);
        hands->load(in);
    }
    else {
        hands = NULL;
    }
    
    //out << "#================================================== legs" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        legs = new Armor();
        
        //legs->save(out);
        legs->load(in);
    }
    else {
        legs = NULL;
    }
    
    //out << "#================================================== feet" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        feet = new Armor();
        
        //feet->save(out);
        feet->load(in);
    }
    else {
        feet = NULL;
    }
    
    //out << "#================================================== rightHand" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        rightHand = new Weapon();
        
        //rightHand->save(out);
        rightHand->load(in);
    }
    else {
        rightHand = NULL;
    }
    
    //out << "#================================================== leftHand" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        leftHand = new Weapon();
        
        //leftHand->save(out);
        leftHand->load(in);
    }
    else {
        leftHand = NULL;
    }
}
