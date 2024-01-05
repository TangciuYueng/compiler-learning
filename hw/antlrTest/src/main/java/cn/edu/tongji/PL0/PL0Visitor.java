package cn.edu.tongji.PL0;
import org.antlr.v4.runtime.tree.ParseTreeVisitor;

/**
 * This interface defines a complete generic visitor for a parse tree produced
 * by {@link PL0Parser}.
 *
 * @param <T> The return type of the visit operation. Use {@link Void} for
 * operations with no return type.
 */
public interface PL0Visitor<T> extends ParseTreeVisitor<T> {
	/**
	 * Visit a parse tree produced by {@link PL0Parser#program}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitProgram(PL0Parser.ProgramContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#subprogram}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSubprogram(PL0Parser.SubprogramContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#constantDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConstantDeclaration(PL0Parser.ConstantDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#constantDefinition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConstantDefinition(PL0Parser.ConstantDefinitionContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#unsignedInteger}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnsignedInteger(PL0Parser.UnsignedIntegerContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#variableDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitVariableDeclaration(PL0Parser.VariableDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#compoundStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCompoundStatement(PL0Parser.CompoundStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#multiStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMultiStatement(PL0Parser.MultiStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStatement(PL0Parser.StatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#assignmentStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAssignmentStatement(PL0Parser.AssignmentStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitExpression(PL0Parser.ExpressionContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#term}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTerm(PL0Parser.TermContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#factor}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFactor(PL0Parser.FactorContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#conditionStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConditionStatement(PL0Parser.ConditionStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#loopStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLoopStatement(PL0Parser.LoopStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#condition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCondition(PL0Parser.ConditionContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#relationalOperator}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitRelationalOperator(PL0Parser.RelationalOperatorContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#emptyStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitEmptyStatement(PL0Parser.EmptyStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#identifier}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIdentifier(PL0Parser.IdentifierContext ctx);
	/**
	 * Visit a parse tree produced by {@link PL0Parser#m}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitM(PL0Parser.MContext ctx);
}