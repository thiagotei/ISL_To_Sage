#include <iostream>
#include <cstdio>
#include <vector>
#include <numeric>
#include <string>
#include <functional>

#include "util.hpp"
#include "SageTransformationWalker.hpp"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

SageTransformationWalker::SageTransformationWalker( ): SageTransformationWalker(false){ }

SageTransformationWalker::SageTransformationWalker( bool verbose ): depth( -1 ), verbose( verbose ), scope_stack() {
  this->scope_stack.push( new SgGlobal() );
}


SgNode* SageTransformationWalker::visit( isl_ast_expr* node ){
  this->depth += 1;
  SgNode* result = NULL;

  switch( isl_ast_expr_get_type(node) ){
    case isl_ast_expr_error:
      result = this->visit_expr_error( node );
      break;

    case isl_ast_expr_op:
      result = this->visit_expr_op( node );
      break;

    case isl_ast_expr_id:
      result = this->visit_expr_id( node );
      break;

    case isl_ast_expr_int:
      result = this->visit_expr_int( node );
      break;

    default:
      result = this->visit_expr_unknown( node );
      break;
  }

  this->depth -= 1;
  return result;
}

SgNode* SageTransformationWalker::visit( isl_ast_node* node ){
  this->depth += 1;
  SgNode* result = NULL;

  switch( isl_ast_node_get_type(node) ){
    case isl_ast_node_error:
      result = this->visit_node_error( node );
      break;

    case isl_ast_node_for:
      result = this->visit_node_for( node );
      break;

    case isl_ast_node_if:
      result = this->visit_node_if( node );
      break;

    case isl_ast_node_block:
      result = this->visit_node_block( node );
      break;

    case isl_ast_node_mark:
      result = this->visit_node_mark( node );
      break;

    case isl_ast_node_user:
      result = this->visit_node_user( node );
      break;

    default:
      result = this->visit_node_unknown( node );
      break;
  }

  this->depth -= 1;
  return result;
}

SgNode* SageTransformationWalker::visit_expr_op(isl_ast_expr* node){
  this->depth += 1;
  SgNode* result = NULL;

  switch( isl_ast_expr_get_op_type(node) ){
    case isl_ast_op_error:
      result = this->visit_op_error( node );
      break;

    case isl_ast_op_and:
      result = this->visit_op_and( node );
      break;

    case isl_ast_op_and_then:
      result = this->visit_op_and_then( node );
      break;

    case isl_ast_op_or:
      result = this->visit_op_or( node );
      break;

    case isl_ast_op_or_else:
      result = this->visit_op_or_else( node );
      break;

    case isl_ast_op_max:
      result = this->visit_op_max( node );
      break;

    case isl_ast_op_min:
      result = this->visit_op_min( node );
      break;

    case isl_ast_op_minus:
      result = this->visit_op_minus( node );
      break;

    case isl_ast_op_add:
      result = this->visit_op_add( node );
      break;

    case isl_ast_op_sub:
      result = this->visit_op_sub( node );
      break;

    case isl_ast_op_mul:
      result = this->visit_op_mul( node );
      break;

    case isl_ast_op_div:
      result = this->visit_op_div( node );
      break;

    case isl_ast_op_fdiv_q:
      result = this->visit_op_fdiv_q( node );
      break;

    case isl_ast_op_pdiv_q:
      result = this->visit_op_pdiv_q( node );
      break;

    case isl_ast_op_pdiv_r:
      result = this->visit_op_pdiv_r( node );
      break;

    case isl_ast_op_zdiv_r:
      result = this->visit_op_zdiv_r( node );
      break;

    case isl_ast_op_cond:
      result = this->visit_op_cond( node );
      break;

    case isl_ast_op_select:
      result = this->visit_op_select( node );
      break;

    case isl_ast_op_eq:
      result = this->visit_op_eq( node );
      break;

    case isl_ast_op_le:
      result = this->visit_op_le( node );
      break;

    case isl_ast_op_lt:
      result = this->visit_op_lt( node );
      break;

    case isl_ast_op_ge:
      result = this->visit_op_ge( node );
      break;

    case isl_ast_op_gt:
      result = this->visit_op_gt( node );
      break;

    case isl_ast_op_call:
      result = this->visit_op_call( node );
      break;

    case isl_ast_op_access:
      result = this->visit_op_access( node );
      break;

    case isl_ast_op_member:
      result = this->visit_op_member( node );
      break;

    case isl_ast_op_address_of:
      result = this->visit_op_address_of( node );
      break;

    default:
      result = this->visit_op_unknown( node );
      break;
  }

  this->depth -= 1;
  return result;
}


list<SgNode*> SageTransformationWalker::visit_expr_operands(isl_ast_expr* node ){
  list<SgNode*> operands_list;

  for( int i = 0; i < isl_ast_expr_get_op_n_arg(node); i += 1 ){
    operands_list.push_back( this->visit_op_operand( node, i ) );
  }

  return operands_list;
}

SgExpression* SageTransformationWalker::visit_op_operand( isl_ast_expr* node, int pos ){
  assert( isl_ast_expr_get_op_n_arg(node) > pos );
  SgExpression* sg_expr = isSgExpression( this->visit( isl_ast_expr_get_op_arg( node, pos ) ) );

  if( sg_expr == NULL ){
    cerr << "Operand was not an SgExpression" << endl;
    abort();
  }

  return sg_expr;
}

SgExpression* SageTransformationWalker::visit_op_lhs( isl_ast_expr* node ){
  return visit_op_operand(node, 0);
}

SgExpression* SageTransformationWalker::visit_op_rhs( isl_ast_expr* node ){
  return visit_op_operand(node, 1);
}

SgExpression* SageTransformationWalker::visit_op_unary_operand( isl_ast_expr* node ){
  return visit_op_operand(node, 0);
}


SgExpression* SageTransformationWalker::visit_op_error(isl_ast_expr* node){
  return NULL;
}

SgExpression*  SageTransformationWalker::visit_op_and( isl_ast_expr* node ){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgBitAndOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation and @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression*  SageTransformationWalker::visit_op_and_then(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgAndOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation and_then @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_or(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgBitOrOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation or @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_or_else(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgOrOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation or_else @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_max(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_min(isl_ast_expr* node){
  return NULL;
}

// Unary minus
SgExpression* SageTransformationWalker::visit_op_minus(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 1 );

  // Get children node
  SgExpression* arg = this->visit_op_unary_operand( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildUnaryExpression<SgMinusOp>( arg );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation minus @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_add(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgAddOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation add @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_sub(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgSubtractOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation sub @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_mul(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgMultiplyOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation mul @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_div(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgDivideOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation div @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_fdiv_q(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_pdiv_q(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_pdiv_r(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_zdiv_r(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_cond(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_select(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_eq(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgEqualityOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation eq @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_le(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgLessOrEqualOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation le @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_lt(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgLessThanOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation lt @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_ge(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgGreaterOrEqualOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation ge @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExpression* SageTransformationWalker::visit_op_gt(isl_ast_expr* node){
  assert( isl_ast_expr_get_op_n_arg(node) == 2 );

  // Get children nodes
  SgExpression* lhs = this->visit_op_lhs( node );
  SgExpression* rhs = this->visit_op_rhs( node );

  // Construct using templated buildBinaryExpression
  SgExpression* exp = buildBinaryExpression<SgGreaterThanOp>( lhs, rhs );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Operation gt @ " << static_cast<void*>(exp) << endl;
  }

  return exp;
}

SgExprStatement* SageTransformationWalker::visit_op_call(isl_ast_expr* node){
  // Get function name
  SgName name( isl_id_get_name( isl_ast_expr_get_id( isl_ast_expr_get_op_arg( node, 0 ) ) ) );

  // Build parameters list
  vector<SgExpression*> parameter_expressions;
  for( int i = 1; i < isl_ast_expr_get_op_n_arg(node); i += 1){
    SgExpression* as_exp = isSgExpression( this->visit( isl_ast_expr_get_op_arg(node, i) ) );

    if( this->verbose ){
      cout << string(this->depth*2, ' ') << "exp @ " << static_cast<void*>(as_exp) << endl;
    }

    if( as_exp == NULL ){
      cerr << "Could not convert argument " << i << " of call " << name.getString() << " to SgExpression." << endl;
      abort();
    }

    parameter_expressions.push_back( as_exp );
  }

  SgExprListExp* parameters = buildExprListExp( parameter_expressions );

  // Build call
  SgScopeStatement* scope = new SgGlobal();
  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "global @ " << static_cast<void*>(scope) << endl;
  }
  if( scope == NULL ){
    cerr << "No scope." << endl;
    abort();
  }

  SgExprStatement* call = buildFunctionCallStmt( name, buildVoidType(), parameters, scope );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Call @ " << static_cast<void*>(call) << endl;
  }

  return call;
}

SgExpression* SageTransformationWalker::visit_op_access(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_member(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_address_of(isl_ast_expr* node){
  return NULL;
}

SgExpression* SageTransformationWalker::visit_op_unknown(isl_ast_expr* node){
  return NULL;
}


SgVarRefExp* SageTransformationWalker::visit_expr_id(isl_ast_expr* node){
  SgName name( isl_id_get_name( isl_ast_expr_get_id(node) ) );
  SgVarRefExp* var_ref = buildVarRefExp( name, this->scope_stack.top() );
  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "id: " << name.getString() << " @ " << var_ref << endl;
  }

  return var_ref;
}

SgIntVal* SageTransformationWalker::visit_expr_int(isl_ast_expr* node){
  isl_val* isl_value = isl_ast_expr_get_val( node );
  long num = isl_val_get_num_si( isl_value );
  long den = isl_val_get_den_si( isl_value );
  assert( den == 1 );

  int int_value = (int) num;
  assert( ((long) int_value) == num );

  SgIntVal* value = buildIntVal( int_value );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "int: " << int_value << " @ " << static_cast<void*>(value) << endl;
  }

  return value;
}

SgNode* SageTransformationWalker::visit_expr_unknown(isl_ast_expr* node){
  return NULL;
}

SgNode* SageTransformationWalker::visit_expr_error(isl_ast_expr* node){
  return NULL;
}


SgNode* SageTransformationWalker::visit_node_error(isl_ast_node* node){
  return NULL;
}

SgNode* SageTransformationWalker::visit_node_for(isl_ast_node* node){
  // Build inititialization statement
  SgStatement* initialization = NULL;
  SgName* name = NULL;
  {
    // Get iterator symbol
    string isl_name = string( isl_id_get_name( isl_ast_expr_get_id( isl_ast_node_for_get_iterator(node) ) ) );
    name = new SgName( isl_name );

    // Get initialization expression
    SgExpression* init_exp = isSgExpression( this->visit( isl_ast_node_for_get_init( node ) ) );

    if( init_exp == NULL ){
      cerr << "Init node in ISL could not be converted to SgExpression" << endl;
      abort();
    }

    // Build variable declaration
    SgAssignInitializer* initalizer = buildAssignInitializer( init_exp, buildIntType() );
    SgVariableDeclaration* var_decl = buildVariableDeclaration( *name, buildIntType(), initalizer, this->scope_stack.top() );

    // Building the variable decl seems sufficient.
    initialization = var_decl;
    if( this->verbose ){
      cout << string(this->depth*2, ' ') << "initalizer @ " << static_cast<void*>(initalizer) << endl;
      cout << string(this->depth*2, ' ') << "var_decl @ " << static_cast<void*>(var_decl) << endl;
      cout << string(this->depth*2, ' ') << "init @ " << static_cast<void*>(initialization) << endl;
    }
  }

  // Get condition expression statment
  SgExprStatement* condition = NULL;
  {
    SgExpression* as_exp = isSgExpression( this->visit( isl_ast_node_for_get_cond( node) ) );

    if( as_exp == NULL ){
      cerr << "Condition node in ISL could not be converted to SgExpression" << endl;
      abort();
    }

    condition = buildExprStatement( as_exp );
    if( this->verbose ){
      cout << string(this->depth*2, ' ') << "cond @ " << static_cast<void*>(condition) << endl;
    }
  }

  // Get increment expression statement
  SgExpression* increment = NULL;
  {

    SgVarRefExp* var_ref = buildVarRefExp( buildInitializedName( *name, buildIntType() ), this->scope_stack.top() );

    // string tab((this->depth+1)*2, ' ');
    // if( this->verbose ) cout << string(this->depth*2, ' ') << "var_ref @ " << static_cast<void*>(var_ref) << endl;
    SgExpression* increment_exp = isSgExpression( this->visit( isl_ast_node_for_get_inc( node ) ) );

    if( increment_exp == NULL ){
      cerr << "Increment node in ISL could not be converted to SgExpression" << endl;
      abort();
    }

    if( this->verbose ){
      cout << string(this->depth*2, ' ') << "exp @ " << static_cast<void*>(increment_exp) << endl;
    }

    // Create increment expression: iterator = iterator + (increment_exp)
    increment = buildBinaryExpression<SgAssignOp>( var_ref, buildBinaryExpression<SgAddOp>( var_ref, increment_exp ) );

    if( this->verbose ){
      cout << string(this->depth*2, ' ') << "increment @ " << static_cast<void*>(increment) << endl;
    }
  }

  SgBasicBlock* body = buildBasicBlock ();

  // Construct for loop node
  SgForStatement* for_stmt = buildForStatement( initialization, condition, increment, body );
  this->scope_stack.push( for_stmt );
  {
    SgStatement* sg_stmt = isSgStatement( this->visit( isl_ast_node_for_get_body( node ) ) );

    if( sg_stmt == NULL ){
      cerr << "Body node in ISL could not be converted to SgStatement" << endl;
      abort();
    }

    if( isSgBasicBlock(sg_stmt) ){
      body = isSgBasicBlock( sg_stmt );
    } else {
      body = isSgBasicBlock( for_stmt->get_loop_body() );

      if( body == NULL ){
        cerr << "Loop body was NOT an SgBasicBlock" << endl;
        abort();
      }
      body->append_statement( sg_stmt );
      sg_stmt->set_parent( body );
    }
    body->set_parent( for_stmt );
    for_stmt->set_loop_body( body );
  }

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "for @ " << static_cast<void*>(for_stmt) << endl;
  }

  this->scope_stack.pop();

  return for_stmt;
}

SgNode* SageTransformationWalker::visit_node_if(isl_ast_node* node){
  SgExpression* condition_node = isSgExpression( this->visit( isl_ast_node_if_get_cond(node) ) );

  if( condition_node == NULL ){
    cerr << "Condition was not an SgExpression" << endl;
    abort();
  }

  SgStatement* then_node = isSgStatement( this->visit( isl_ast_node_if_get_then(node) ) );

  if( then_node == NULL ){
    cerr << "Then was not an SgStatement" << endl;
    abort();
  }

  // Always wrap statements as blocks
  if( !isSgBasicBlock(then_node) ){
    SgBasicBlock* block = buildBasicBlock( then_node );
    then_node = block;
  }

  SgStatement* else_node = NULL;
  if( isl_ast_node_if_has_else( node ) ){
    else_node = isSgStatement( this->visit( isl_ast_node_if_get_else(node) ) );
    if( else_node == NULL ){
      cerr << "Else was not an SgStatement" << endl;
      abort();
    }

    if( isSgBasicBlock( else_node) ){
      SgBasicBlock* block = buildBasicBlock( else_node );
      else_node = block;
    }
  }

  SgIfStmt* if_stmt = buildIfStmt( condition_node, then_node, else_node );

  return if_stmt;
}

SgNode* SageTransformationWalker::visit_node_block(isl_ast_node* node){
  SgBasicBlock* block = buildBasicBlock();

  this->scope_stack.push( block );

  if( this->verbose ){
    cout << string(this->depth*2, ' ') << "Block @ "<< static_cast<void*>(block) << endl;
  }

  isl_ast_node_list* list = isl_ast_node_block_get_children(node);
  for( int i = 0; i < isl_ast_node_list_n_ast_node(list); i += 1 ){
    isl_ast_node* node = isl_ast_node_list_get_ast_node(list, i);
    SgStatement* sg_stmt = isSgStatement( this->visit( node ) );

    if( sg_stmt == NULL ){
      cerr << "Could not convert node to SgStatement" << endl;
      abort();
    }

    block->append_statement( sg_stmt );
  }

  this->scope_stack.pop();

  return block;
}

SgNode* SageTransformationWalker::visit_node_mark(isl_ast_node* node){
  return NULL;
}

SgNode* SageTransformationWalker::visit_node_user(isl_ast_node* node){
  SgNode* inner = this->visit( isl_ast_node_user_get_expr(node) );

  return inner;
}

SgNode* SageTransformationWalker::visit_node_unknown(isl_ast_node* node){
  return NULL;
}