#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/** Category: SCDE Core Fn
 * -------------------------------------------------------------------------------------------------
 *  FName: Device-Specifications to Array
 *  Desc: Returns all (multiple) definitions (name) that match the given devicespecification (devspec)
 *  Info: devspec should contain data (check in advance!) ; 
 *  Para: const xString_s devspecString -> the devicespecification (devspec) string
 *  Rets: xHeadMultipleStringSLTQ_t -> singly linked tail queue head to store multiple strings
 *        (definition-names) that match the requested devicespecification (devspec),
 *        loop string entrys till STAILQ_EMPTY
 * -------------------------------------------------------------------------------------------------
 */
struct xHeadMultipleStringSLTQ_s
Devspec2Array(const xString_t devspecString)
{
 // prepare STAILQ head for multiple definitions storage
  struct xHeadMultipleStringSLTQ_s definitionHeadMultipleStringSLTQ;

  // Initialize the queue head
  STAILQ_INIT(&definitionHeadMultipleStringSLTQ);

//---------------------------------------------------------------------------------------------------

// CODE HERE IS NOT COMPLETE - ONLY FOR DEBUGGING

  // alloc an definitionMultipleString queue element
  xMultipleStringSLTQE_t *definitionMultipleStringSLTQE =
	malloc(sizeof(xMultipleStringSLTQE_t));

  // fill string in queue element 
  definitionMultipleStringSLTQE->string.length =
	asprintf(&definitionMultipleStringSLTQE->string.characters
		,"%.*s"
		,devspecString.length
		,devspecString.characters);

  // insert definitionMultipleString queue element in stail-queue
  STAILQ_INSERT_TAIL(&definitionHeadMultipleStringSLTQ, definitionMultipleStringSLTQE, entries);

//---------------------------------------------------------------------------------------------------

  // return STAILQ head, stores multiple (all) matching definitions, if STAILQ_EMPTY -> no matching definitions
  return definitionHeadMultipleStringSLTQ;
}
