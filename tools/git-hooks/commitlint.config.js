const COMMIT_MESSAGE_SEPARATOR = ':'
const COMMIT_TASK_STATUS_PATTERN = /\[.*\]/gi
const COMMIT_TYPE_SCOPE_SEPARATOR = {
  start: '(',
  end: ')',
}
const COMMIT_STATUS_SEPARATORS = {
  start: '[',
  end: ']',
}
const COMMIT_DESCRIPTION_SEPARATOR = '\n'
const USAGE_MSG = '请按commit-message规范填写"[JIRA-ID] type(<scope>): <subject>"'
const TYPE_FORMAT = 'feat|fix|docs|style|refactor|perf|test|chore'
const SCOPE_FORMAT = 'cc|sample|ir0|runtime|tools|meta|python|ir1|ops|translation|tfu|devops|test|perf|common|doc'
const gCheckScope = true

function isInvalid(obj) {
  // Chinese character test
  var fmt = /^[^\u4e00-\u9fa5]+$/
  if (typeof obj == "undefined" || obj == null || obj == "" || !fmt.test(obj)) {
    return true
  } else {
    return false
  }
}

function isTypeValid(type) {
  var fmt = RegExp('^(' + TYPE_FORMAT + ')$')
  return fmt.test(type)
}

function isScopeValid(scopes) {
  var fmt = RegExp('^(' + SCOPE_FORMAT + ')$')
  var scopeArr = scopes.split('|')
  for(var i=0; i<scopeArr.length; ++i) {
    if(!scopeArr[i] || scopeArr[i].length == 0 || !fmt.test(scopeArr[i])) {
      return false;
    }
  }
  return true;
}

function parseCommit(commitMsg) {
  // [                 JiraDest                   ]commitHeader:commitSubject
  // [commitTaskId0, commitTaskId1...commitTaskIdx]type(scope):commitSubject
  // [jiraId] type(<scope>): <subject>
  const commitMessage = commitMsg
    .split(COMMIT_DESCRIPTION_SEPARATOR)
    .filter(commitMessageSeparatedPart => commitMessageSeparatedPart)[0]
  const commitMessageParts = commitMessage.split(COMMIT_MESSAGE_SEPARATOR)

  const rawCommitHeader =
  commitMessageParts.length >= 2 ? commitMessageParts[0] : ''

  const commitHeader = rawCommitHeader
    .replace(COMMIT_TASK_STATUS_PATTERN, '')
    .trim()

  const commitTypeScope = commitHeader.split(COMMIT_TYPE_SCOPE_SEPARATOR.start)
  const commitType = commitTypeScope.length > 1
    ? commitTypeScope[0].trim()
    : ''

  const commitScope = commitTypeScope.length > 1
    ? commitTypeScope[1].replace(COMMIT_TYPE_SCOPE_SEPARATOR.end, '').trim()
    : ''
  
  const commitSubject =
    commitMessageParts.length > 2
    ? commitMessageParts
    .filter((_value, index) => index > 0)
    .join(COMMIT_MESSAGE_SEPARATOR)
    .trim()
    : commitMessageParts[commitMessageParts.length - 1].trim() 

  const rawCommitStatus = rawCommitHeader.split(COMMIT_STATUS_SEPARATORS.end)
  const jiraDesc = rawCommitStatus.slice(0, rawCommitStatus.length-1)
  const commitTaskIds = jiraDesc
    .map(value => value.replace(COMMIT_STATUS_SEPARATORS.start, '').trim())
  return {
    task_ids:commitTaskIds,
    type:commitType,
    scope:commitScope,
    subject:commitSubject
  }
}

function checkCommit(msg) {
  var errorList = []
  // Revert
  if ((/^(Revert|Merge|merge).*/.test(msg.subject))) {
    return errorList
  }
  if (isInvalid(msg.type)) {
    errorList.push('type非法')
  } else if (!isTypeValid(msg.type)) {
    errorList.push('type(' + msg.type + ')类型须为以下关键字之一：' + TYPE_FORMAT)
  }

  if (isInvalid(msg.scope)) {
    errorList.push('scope非法')
  } else if (gCheckScope && !isScopeValid(msg.scope)) {
    errorList.push('scope(' + msg.scope + ')模块须为以下关键字之一：\n\t' + SCOPE_FORMAT)
  }

  if (isInvalid(msg.subject)) {
    errorList.push('subjec非法')
  } else {
    if (/\.$/.test(msg.subject)) {
      errorList.push('subject(' + msg.subject + ')句尾存在冗余句号')
    }
    if (/^[A-Z].*/.test(msg.subject)) {
      errorList.push('subject(' + msg.subject + ')首字母须小写')
    }
  }
  return errorList
}

module.exports = {
  extends: ['@commitlint/config-conventional'],
  plugins: ['commitlint-plugin-function-rules'],
  rules: {
    'type-empty': [0],
    'subject-empty': [0],
    'subject-case': [0],
    'type-case': [0],
    'type-enum': [0],
    'header-max-length' : [2, 'always', 128],
    // do a hook of 'type-enum' to check message
    'function-rules/type-enum': [
      2,
      'always',
      (parsed) => {

        const commitMsg = parsed.raw.trim()
        // empty message
        if (!commitMsg) {
          return [false, '请输入提交信息,不能为空']
        }

        msg = parseCommit(commitMsg)
        errorList = checkCommit(msg)
        if (errorList.length > 0) {
          errors = '\ncommit发现' + errorList.length + '个错误:\n'
          for (var i = 0; i < errorList.length; i++) {
            errors += '  ' + i + ") " + errorList[i] + '\n'
          }
          return [false, errors]
        } else {
          return [true]
        }
      }
    ],
  },
  helpUrl:
    'http://gitlab.software.cambricon.com/neuware/magicmind/-/raw/master/tools/git-commit-template',
}
