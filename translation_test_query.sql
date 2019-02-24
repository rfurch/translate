SELECT t.translation FROM translations t 
LEFT JOIN languages l ON t.toLanguageID=l.id
LEFT JOIN expressionsCache e ON t.expressionID=e.id
WHERE l.alias='es' AND e.englishExpression="Another one to the local cache";

SELECT t.translation FROM translations t 
LEFT JOIN languages l ON t.toLanguageID=l.id
LEFT JOIN expressionsCache e ON t.expressionID=e.id
WHERE l.alias='de' AND e.englishExpression="I need my own translation for this sentence";

SELECT t.translation FROM translations t 
LEFT JOIN languages l ON t.toLanguageID=l.id
LEFT JOIN expressionsCache e ON t.expressionID=e.id
WHERE l.alias='es' AND e.englishExpression="I need my own translation for this sentence";

