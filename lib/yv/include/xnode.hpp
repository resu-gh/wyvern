#pragma once

#include "glogger.hpp"
#include "xaction.hpp"
#include "xnodec.hpp"
#include "xnodetype.hpp"
#include "xtoken.hpp"

#include <climits>
#include <memory>
#include <set>
#include <vector>

class xnode : public std::enable_shared_from_this<xnode> {
  private:
    /// node index
    int m_index;
    /// node type
    xnodetype m_type;
    /// first char in the interval of chars represented by the node
    int m_begin_char;
    /// one past the last char in the interval of chars represented by the node
    int m_end_char;
    /// the child nodes
    std::vector<std::shared_ptr<xnode>> m_nodes;
    /// true if this node is nullable, false otherwise
    bool m_nullable;
    /// the token recognized at the node or null if the node doesn't recognize a token
    std::shared_ptr<xtoken> m_token;
    /// the action taken at the node or null if no action is taken at the node
    std::shared_ptr<xaction> m_action;
    /// the first positions at the node
    std::set<std::shared_ptr<xnode>, xnodec> m_first_positions;
    /// the last positions at the node
    std::set<std::shared_ptr<xnode>, xnodec> m_last_positions;
    /// the follow positions at the node
    std::set<std::shared_ptr<xnode>, xnodec> m_follow_positions;
    /// logger
    glogger m_log;

  public:
    xnode(int index, xnodetype type);
    xnode(int index, int begin_char, int end_char);
    xnode(int index, int begin_char, int end_char, const std::shared_ptr<xtoken> &token);
    xnode(int index, const std::shared_ptr<xaction> &action);

  public:
    std::shared_ptr<xnode> self();

  public:
    int index() const;
    xnodetype type() const;
    bool is_nullable() const;

  public:
    std::string lexeme() const;

  public:
    void add_node(const std::shared_ptr<xnode> &node);

  public:
    void calculate_nullable();
    void calculate_first_positions();
    void calculate_last_positions();
    void calculate_follow_positions();

  public:
    bool operator<(const xnode &node) const;

  public:
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;

  public:
    static const int BEGIN_CHARACTER = 0;
    static const int END_CHARACTER = INT_MAX;
    static const int INVALID_BEGIN_CHARACTER = -2;
    static const int INVALID_END_CHARACTER = -1;
};
