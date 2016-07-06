#include <cassert>
#include <utility>
#include <array>
#include <vector>
#include <string>
#include <iostream>

#include "rose.h"
#include "all_isl.hpp"
#include "SageTransformationWalker.hpp"
#include "PrintNodeWalker.hpp"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

isl_union_set* domain_from_domains( isl_ctx* ctx, vector<string> domains ){
  assert( !domains.empty() );

  auto iter = domains.begin();
  isl_union_set* domain = isl_union_set_read_from_str(ctx, (*iter).c_str() );
  ++iter;

  for( ; iter != domains.end(); ++iter ){
    domain = isl_union_set_union( domain, isl_union_set_read_from_str(ctx, (*iter).c_str() ) );
  }

  return domain;
}

isl_union_map* map_from_maps( isl_ctx* ctx, vector<string> maps ){
  assert( !maps.empty() );

  auto iter = maps.begin();
  isl_union_map* map = isl_union_map_read_from_str(ctx, (*iter).c_str() );
  ++iter;

  for( ; iter != maps.end(); ++iter ){
    map = isl_union_map_apply_range( map, isl_union_map_read_from_str(ctx, (*iter).c_str() ) );
  }

  return map;
}

void example( char** argv, vector<string> domains, vector<string> maps ){
  // Produce ISL AST
  isl_ast_node* isl_ast;
  {
    isl_ctx* ctx = isl_ctx_alloc();
    isl_union_set* domain = domain_from_domains(ctx, domains );
    isl_union_map* schedule = map_from_maps(ctx, maps );
    isl_ast_build* build;
    isl_printer* p;

    schedule = isl_union_map_intersect_domain(schedule, domain);

    build = isl_ast_build_alloc(ctx);
    isl_ast = isl_ast_build_node_from_schedule_map(build, schedule);
    isl_ast_build_free(build);

    cout << "Their Printer:" << endl;
    p = isl_printer_to_file(ctx, stdout);
    p = isl_printer_set_output_format(p, ISL_FORMAT_C);
    p = isl_printer_print_ast_node(p, isl_ast);
    cout << endl;
    isl_printer_free(p);

    // isl_ast_node_free(tree);
    // isl_ctx_free(ctx);
  }

  {
    PrintNodeWalker walker;
    cout << "PrintNodeWalker:" << endl;
    cout << walker.visit( isl_ast ) << endl;
  }

  {
    cout << "SageTransformationWalker:" << endl;
    bool verbose = true;

    // "Sorry, not implemented" as of rose -v 0.9.7.39 (6.23.2016) ???
    //setSourcePositionClassificationMode( e_sourcePositionCompilerGenerated );

    // Write out a template file.
    ofstream template_file;
    //string template_code( "#include <iostream>\nusing namespace std;\nint main(){ {int A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;\nA = (B = (C = (D = (E = (F = (G = (H = (I = (J = (K = (L = (M = (N = (O = (P = (Q = (R = (S = (T = (U = (V = (W = (X = (Y = (Z = (a = (b = (c = (d = (e = (f = (g = (h = (i = (j = (k = (l = (m = (n = (o = (p = (q = (r = (s = (t = (u = (v = (w = (x = (y = (z = 1234))))))))))))))))))))))))))))))))))))))))))))))))))); cout << A << B << C << D << E << F << G << H << I << J << K << L << M << N << O << P << Q << R << S << T << U << V << W << X << Y << Z << a << b << c << d << e << f << g << h << i << j << k << l << m << n << o << p << q << r << s << t << u << v << w << x << y << z << endl; }{int A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;\nA = (B = (C = (D = (E = (F = (G = (H = (I = (J = (K = (L = (M = (N = (O = (P = (Q = (R = (S = (T = (U = (V = (W = (X = (Y = (Z = (a = (b = (c = (d = (e = (f = (g = (h = (i = (j = (k = (l = (m = (n = (o = (p = (q = (r = (s = (t = (u = (v = (w = (x = (y = (z = 1234))))))))))))))))))))))))))))))))))))))))))))))))))); cout << A << B << C << D << E << F << G << H << I << J << K << L << M << N << O << P << Q << R << S << T << U << V << W << X << Y << Z << a << b << c << d << e << f << g << h << i << j << k << l << m << n << o << p << q << r << s << t << u << v << w << x << y << z << endl; } }");
    string template_code( "#include <iostream>\nusing namespace std;\nint main(){ }");
    std::string template_file_name( "__template_file__.cpp" );

    template_file.open( template_file_name.c_str(), ios::trunc | ios::out );
    assert( template_file.is_open() );

    template_file << template_code << endl;

    template_file.close();

    vector<string> project_argv;
    // Apparently it is necessary to have the executable name in the arguments.
    project_argv.push_back( string(argv[0]) );
    project_argv.push_back( template_file_name );
    if( verbose ) cout << "Calling Frontend" << endl;
    SgProject* project = frontend( project_argv );
    if( verbose ) cout << "Parsed template file" << endl;

    // Find the existing declaration node in the tree.
    // We need the global scope produced by the source file.
    // This is _not_ what is returned by SgProject.get_globalScopeAcrossFiles()
    SgFunctionDeclaration* target_decl = findFunctionDeclaration( project, "main", NULL, true);

    // Run ISL -> Sage walker over ISL tree, rendering it into Sage,
    SageTransformationWalker walker(verbose);

    SgNode* walker_rendered_ast = walker.visit( isl_ast );

    // 'Cast' into SgStatement (there is not reason, in my mind why this should fail)
    SgStatement* body_stmt = isSgStatement( walker_rendered_ast );
    if( body_stmt == NULL ){
      cerr << "Could not convert ISL AST into SgStatement" << endl;
      abort();
    }

    if( verbose ) cout << "Inserting into main()" << endl;
    target_decl->get_definition()->append_statement( body_stmt );

    //if( verbose ) cout << "AST Post Porcessing" << endl;
    //AstPostProcessing( project );

    if( verbose ) cout << "Fixing Var References" << endl;
    fixVariableReferences( body_stmt );

    // Write AST to dot file
    if( verbose ) cout << "Writing to dot file" << endl;
    generateDOT( *project );
    if( verbose ) cout << "Unparsing" << endl;
    project->unparse();

    cout << "Generated Code:" << endl;
    string line;
    ifstream rose_output( (string("rose_") + template_file_name).c_str() );
    if( rose_output.is_open() ){
      while( getline( rose_output, line ) != NULL ){
        cout << line << endl;
      }
      rose_output.close();
    }
  }

}


int main( int argc, char** argv){
  vector< pair< vector<string>, vector<string> > > tests;
  /*
  {
    vector<string> domains = {
      string( "{S[i] : 1 <= i <= 10}")
    };

    vector<string> maps = {
      string( "{S[i] -> [0,i,0]}" )
    };

    tests.push_back( make_pair( domains, maps) );
  }
  //*/
  /*
  {
    vector<string> domains = {
      string( "{S1[i] : 1 <= i <= 10}"),
      string( "{S2[j] : 1 <= j <= 10}")
    };

    vector<string> maps = {
      string( "{S1[i] -> [0,i,0] ; S2[j] -> [1,j,0]}" )
    };

    tests.push_back( make_pair( domains, maps) );
  }
  //*/

  //*
  {
    vector<string> domains = {
      string( "{S1[i] : 1 <= i <= 10}"),
      string( "{S2[j] : 1 <= j <= 10}")
    };

    vector<string> maps = {
      string( "{S1[i] -> [0,i,0] ; S2[j] -> [0,j,1]}" )
    };

    tests.push_back( make_pair( domains, maps) );
  }
  //*/

  /*
  {
    vector<string> domains = {
      string( "{ S1[i] : 1 <= i <= 10; S2[i,j] : 1 <= i <= 10 and 1 <= j <= 100 }")
    };

    vector<string> maps = {
      string( "{S1[i] -> [0,i,0,0] ; S2[i,j] -> [0,i,j,0]}" )
    };

    tests.push_back( make_pair( domains, maps) );
  }
  //*/

  cout << "===============================================\n" << std::endl;
  for( auto iter = tests.begin(); iter != tests.end(); ++iter ){
    auto doms = (*iter).first;
    auto maps = (*iter).second;
    example( argv, doms, maps );
    cout << "\n===============================================\n" << std::endl;
  }

  return 0;
}