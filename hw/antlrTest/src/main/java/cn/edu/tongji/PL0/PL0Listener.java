package cn.edu.tongji.PL0;
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link PL0Parser}.
 */
public interface PL0Listener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link PL0Parser#program}.
	 * @param ctx the parse tree
	 */
	void enterProgram(PL0Parser.ProgramContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#program}.
	 * @param ctx the parse tree
	 */
	void exitProgram(PL0Parser.ProgramContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#subprogram}.
	 * @param ctx the parse tree
	 */
	void enterSubprogram(PL0Parser.SubprogramContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#subprogram}.
	 * @param ctx the parse tree
	 */
	void exitSubprogram(PL0Parser.SubprogramContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#constantDeclaration}.
	 * @param ctx the parse tree
	 */
	void enterConstantDeclaration(PL0Parser.ConstantDeclarationContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#constantDeclaration}.
	 * @param ctx the parse tree
	 */
	void exitConstantDeclaration(PL0Parser.ConstantDeclarationContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#constantDefinition}.
	 * @param ctx the parse tree
	 */
	void enterConstantDefinition(PL0Parser.ConstantDefinitionContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#constantDefinition}.
	 * @param ctx the parse tree
	 */
	void exitConstantDefinition(PL0Parser.ConstantDefinitionContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#unsignedInteger}.
	 * @param ctx the parse tree
	 */
	void enterUnsignedInteger(PL0Parser.UnsignedIntegerContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#unsignedInteger}.
	 * @param ctx the parse tree
	 */
	void exitUnsignedInteger(PL0Parser.UnsignedIntegerContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#variableDeclaration}.
	 * @param ctx the parse tree
	 */
	void enterVariableDeclaration(PL0Parser.VariableDeclarationContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#variableDeclaration}.
	 * @param ctx the parse tree
	 */
	void exitVariableDeclaration(PL0Parser.VariableDeclarationContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#compoundStatement}.
	 * @param ctx the parse tree
	 */
	void enterCompoundStatement(PL0Parser.CompoundStatementContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#compoundStatement}.
	 * @param ctx the parse tree
	 */
	void exitCompoundStatement(PL0Parser.CompoundStatementContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#statement}.
	 * @param ctx the parse tree
	 */
	void enterStatement(PL0Parser.StatementContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#statement}.
	 * @param ctx the parse tree
	 */
	void exitStatement(PL0Parser.StatementContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#assignmentStatement}.
	 * @param ctx the parse tree
	 */
	void enterAssignmentStatement(PL0Parser.AssignmentStatementContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#assignmentStatement}.
	 * @param ctx the parse tree
	 */
	void exitAssignmentStatement(PL0Parser.AssignmentStatementContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#expression}.
	 * @param ctx the parse tree
	 */
	void enterExpression(PL0Parser.ExpressionContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#expression}.
	 * @param ctx the parse tree
	 */
	void exitExpression(PL0Parser.ExpressionContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#term}.
	 * @param ctx the parse tree
	 */
	void enterTerm(PL0Parser.TermContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#term}.
	 * @param ctx the parse tree
	 */
	void exitTerm(PL0Parser.TermContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#factor}.
	 * @param ctx the parse tree
	 */
	void enterFactor(PL0Parser.FactorContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#factor}.
	 * @param ctx the parse tree
	 */
	void exitFactor(PL0Parser.FactorContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#conditionStatement}.
	 * @param ctx the parse tree
	 */
	void enterConditionStatement(PL0Parser.ConditionStatementContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#conditionStatement}.
	 * @param ctx the parse tree
	 */
	void exitConditionStatement(PL0Parser.ConditionStatementContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#loopStatement}.
	 * @param ctx the parse tree
	 */
	void enterLoopStatement(PL0Parser.LoopStatementContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#loopStatement}.
	 * @param ctx the parse tree
	 */
	void exitLoopStatement(PL0Parser.LoopStatementContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#condition}.
	 * @param ctx the parse tree
	 */
	void enterCondition(PL0Parser.ConditionContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#condition}.
	 * @param ctx the parse tree
	 */
	void exitCondition(PL0Parser.ConditionContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#relationalOperator}.
	 * @param ctx the parse tree
	 */
	void enterRelationalOperator(PL0Parser.RelationalOperatorContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#relationalOperator}.
	 * @param ctx the parse tree
	 */
	void exitRelationalOperator(PL0Parser.RelationalOperatorContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#emptyStatement}.
	 * @param ctx the parse tree
	 */
	void enterEmptyStatement(PL0Parser.EmptyStatementContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#emptyStatement}.
	 * @param ctx the parse tree
	 */
	void exitEmptyStatement(PL0Parser.EmptyStatementContext ctx);
	/**
	 * Enter a parse tree produced by {@link PL0Parser#identifier}.
	 * @param ctx the parse tree
	 */
	void enterIdentifier(PL0Parser.IdentifierContext ctx);
	/**
	 * Exit a parse tree produced by {@link PL0Parser#identifier}.
	 * @param ctx the parse tree
	 */
	void exitIdentifier(PL0Parser.IdentifierContext ctx);
}