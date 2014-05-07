#include "OverdrawManager.h"


OverdrawManager::OverdrawManager() {
}

OverdrawManager::~OverdrawManager() {
  clear();
}

void OverdrawManager::setBlock(const v3di_t& position, BYTE blockType) {
//  OverdrawColumn *overdrawColumn = getOverdrawColumn(
//  setOverdrawBlock(getOver
}

OverdrawBlock *OverdrawManager::removeColumn(int i, int j) {
}

void OverdrawManager::clear() {
}

void OverdrawManager::save(FILE* file) {
}

void OverdrawManager::load(FILE* file) {
}

OverdrawColumn* OverdrawManager::getOverdrawColumn(int i, int j) {
}

OverdrawColumn* OverdrawManager::createOverdrawColumn(int i, int j) {
}

void OverdrawManager::setOverdrawBlock(OverdrawColumn* overdrawColumn, OverdrawBlock* overdrawBlock) {
}
