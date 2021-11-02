#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPEXTRACTOR_H_

#include <model/Entity.h>
#include <component/PKB/PKB.h>

class NextBipExtractor {
 public:
  NextBipExtractor() = default;
  explicit NextBipExtractor(PKB* pkb);
  std::vector<Entity*> GetNextBip(const std::string& target);
  std::vector<Entity*> GetPrevBip(const std::string& target);
  std::vector<Entity*> GetAllNextBipRHS();
  std::vector<Entity*> GetAllNextBipLHS();
  std::vector<std::tuple<Entity*, Entity*>> GetAllNextBip();
  std::vector<std::tuple<Entity*, Entity*>> GetAllPrevBip();
  bool HasNextBip();
  bool HasPrevBip();
  bool HasNextBip(std::string target);
  bool HasPrevBip(std::string target);
  bool HasNextBip(std::string first, std::string second);

 private:
  PKB* pkb_{};
  std::vector<Entity*> stmt_list_;
  std::vector<Procedure*> proc_list_;
  std::vector<Entity*> call_list_;
  std::unordered_map<Entity*, std::list<Entity*>*>* next_bip_map_; // maps nextbip of stmt num to list of stmts
  std::unordered_map<Entity*, std::list<Entity*>*>* prev_bip_map_;
  bool populated_ = false;

  std::vector<Entity*> GetRelationship(PKBRelRefs rel, const std::string& target);
  void PopulateBipMaps();
  void JoinEndToEnd(Procedure* called_proc, const std::list<Entity*> &next_entities);
  void JoinStartToStart(Procedure* called_proc, Entity* prev_entity);
  std::list<Entity*> GetRelFromMap(Entity* target, PKBRelRefs rel);
  void AddNext(Entity* prev_entity, Entity* next_entity);
  static void AddRelationship(Entity* first_arg,
                              Entity* second_arg,
                              std::unordered_map<Entity*, std::list<Entity*>*>* map);
  void EraseNextRelationship(Entity* entity);
  void ErasePrevRelationship(Entity* next_stmt, Entity* prev_stmt);
  std::list<int> GetBipLastStmts(Block* block);
  std::list<int> HandleCallLastStmt(const std::list<int>& last_stmts);
  std::unordered_map<Entity*, std::list<Entity*>*>* ConvertPKBMap(const std::unordered_map<std::string,
                                                                                     std::vector<Entity*>>& pkb_map);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPEXTRACTOR_H_
