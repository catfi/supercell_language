#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "language/tree/ASTNodeFactory.h"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::ASTNode            );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Annotation         );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Annotations        );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Internal           );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Program            );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Package            );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Import             );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Block              );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Identifier         );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::SimpleIdentifier   );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::NestedIdentifier   );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::TemplatedIdentifier);
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Literal            );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::NumericLiteral     );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::StringLiteral      );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::ObjectLiteral      );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::TypeSpecifier      );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::FunctionType       );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Declaration        );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::ClassDecl          );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::EnumDecl           );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::InterfaceDecl      );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::TypedefDecl        );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::FunctionDecl       );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::VariableDecl       );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Statement          );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::DeclarativeStmt    );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::ExpressionStmt     );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::IterativeStmt      );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::ForStmt            );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::ForeachStmt        );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::WhileStmt          );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::SelectionStmt      );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::IfElseStmt         );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::SwitchStmt         );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::BranchStmt         );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::Expression         );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::PrimaryExpr        );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::UnaryExpr          );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::BinaryExpr         );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::TernaryExpr        );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::MemberExpr         );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::CallExpr           );
BOOST_CLASS_EXPORT_IMPLEMENT(zillians::language::tree::CastExpr           );

